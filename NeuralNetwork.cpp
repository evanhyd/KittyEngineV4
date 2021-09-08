#include "NeuralNetwork.h"
#include <iostream>
#include <fstream>
#include <cassert>

NeuralNetwork::NeuralNetwork(const std::vector<int>& topology) : RMS(0.0)
{
	//topology's size is the number of layer
	for (int layer_index = 0; layer_index < topology.size(); ++layer_index)
	{
		//add a new layer
		this->layers.push_back(Layer());


		//output layer has no connections
		int output_num = (layer_index == topology.size() - 1 ? 0 : topology[layer_index + 1]);

		//populate the neurons, + 1 for the bias neuron
		for (int neuron_index = 0; neuron_index < topology[layer_index] + 1; ++neuron_index)
		{
			this->layers.back().push_back(Neuron(neuron_index, output_num));
		}

		//set bias neuron's value to 1
		this->layers.back().back().SetValue(1.0);
	}
}


void NeuralNetwork::ForwardPropagate(const std::vector<double>& features)
{
	Layer& input_layer = this->layers.front();

	for (int neuron_index = 0; neuron_index < features.size(); ++neuron_index)
	{
		input_layer[neuron_index].SetValue(features[neuron_index]);
	}

	//forward propagate
	for (int layer_index = 1; layer_index < this->layers.size(); ++layer_index)
	{
		Layer& curr_layer = this->layers[layer_index];
		const Layer& previous_layer = this->layers[layer_index - 1]; 

		//feedforward all non-bias neurons
		for (int neuron_index = 0; neuron_index < curr_layer.size() - 1; ++neuron_index)
		{
			curr_layer[neuron_index].FeedForwardFrom(previous_layer);
		}
	}
}


void NeuralNetwork::BackPropagate(const std::vector<double>& labled_examples)
{
	//calculate the mean of square loss
	Layer& output_layer = this->layers.back();
	
	//square root of mean error
	this->RMS = 0.0;

	//- 1 to skip the bias neuron
	for (int neuron_index = 0; neuron_index < output_layer.size() - 1; ++neuron_index)
	{
		double loss = labled_examples[neuron_index] - output_layer[neuron_index].GetValue();
		this->RMS += loss * loss;
	}

	this->RMS /= double(output_layer.size() - 1);
	this->RMS = sqrt(RMS);

	//calculate the output layer gradient
	for (int neuron_index = 0; neuron_index < output_layer.size() - 1; ++neuron_index)
	{
		output_layer[neuron_index].UpdateOutputLayerGradient(labled_examples[neuron_index]);
	}

	//calculate the hidden layer gradient
	for (int layer_index = this->layers.size() - 2; layer_index > 0; --layer_index)
	{
		Layer& curr_layer = this->layers[layer_index];
		const Layer& next_layer = this->layers[layer_index + 1];

		for (Neuron& curr_neuron : curr_layer)
		{
			curr_neuron.UpdateHiddenLayerGradient(next_layer);
		}
	}


	//update the weight
	for (int layer_index = this->layers.size() - 1; layer_index > 0; --layer_index)
	{
		Layer& curr_layer = this->layers[layer_index];
		Layer& prev_layer = this->layers[layer_index - 1];

		for (int neuron_index = 0; neuron_index < curr_layer.size() - 1; ++neuron_index)
		{
			curr_layer[neuron_index].UpdateLayerWeight(prev_layer);
		}
	}
}


std::vector<double> NeuralNetwork::GetResult() const
{
	const Layer& output_layer = this->layers.back();
	std::vector<double> results(output_layer.size() - 1);

	//-1 to skip the bias neuron
	for (int neuron_index = 0; neuron_index < output_layer.size() - 1; ++neuron_index)
	{
		results[neuron_index] = output_layer[neuron_index].GetValue();
	}

	return results;
}

double NeuralNetwork::GetRMS() const
{
	return this->RMS;
}


void NeuralNetwork::SaveNeuralNetwork(const std::string& file_name) const
{
	std::ofstream output(file_name, std::ofstream::trunc | std::ofstream::binary);

	for (const Layer& layer : this->layers)
	{
		for (const Neuron& neuron : layer)
		{
			output.write(reinterpret_cast<const char*>(neuron.output_weights.data()), neuron.output_weights.size() * sizeof(Connection));
		}
	}

	output.close();
}

bool NeuralNetwork::LoadNeuralNetwork(const std::string& file_name)
{
	std::ifstream input(file_name, std::ifstream::binary);
	if (!input.is_open()) return false;

	for (Layer& layer : this->layers)
	{
		for (Neuron& neuron : layer)
		{
			input.read(reinterpret_cast<char*>(neuron.output_weights.data()), neuron.output_weights.size() * sizeof(Connection));
		}
	}

	input.close();
	return true;
}