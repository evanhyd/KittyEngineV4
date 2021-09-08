#include "Neuron.h"
#include "NeuralNetwork.h"
#include <algorithm>
#include <iostream>

Neuron::Neuron(int new_neuron_index, int output_num) : this_neuron_index(new_neuron_index), value(0.0), gradient(0.0)
{
	this->output_weights.assign(output_num, Connection());
}

void Neuron::SetValue(double new_value)
{
	this->value = new_value;
}
double Neuron::GetValue() const
{
	return this->value;
}


void Neuron::FeedForwardFrom(const Layer& previous_layer)
{
	this->value = 0.0;

	for (const Neuron& prev_neuron : previous_layer)
	{
		this->value += prev_neuron.value * prev_neuron.output_weights[this->this_neuron_index].weight;
	}

	this->value = Neuron::TransferFunction(this->value);
}

void Neuron::UpdateOutputLayerGradient(double target_value)
{
	double loss = target_value - this->value;
	this->gradient = loss * Neuron::TransferFunctionDerv(this->value);
}

void Neuron::UpdateHiddenLayerGradient(const Layer& next_layer)
{
	//sum of derivative of weight
	double sum_DOW = 0.0;
	for (int neuron_index = 0; neuron_index < next_layer.size() - 1; ++neuron_index)
	{
		sum_DOW += this->output_weights[neuron_index].weight * next_layer[neuron_index].gradient;
	}

	this->gradient = sum_DOW * Neuron::TransferFunctionDerv(this->value);
}

void Neuron::UpdateLayerWeight(Layer& prev_layer)
{
	for (Neuron& prev_neuron : prev_layer)
	{
		double old_diff_weight = prev_neuron.output_weights[this->this_neuron_index].diff_weight;
		double new_diff_weight = prev_neuron.value * this->gradient * LEARNING_RATE + old_diff_weight * MOMENTUM_RATE;

		prev_neuron.output_weights[this->this_neuron_index].diff_weight = new_diff_weight;
		prev_neuron.output_weights[this->this_neuron_index].weight += new_diff_weight;
	}
}

double Neuron::TransferFunction(double value)
{
	//sigmoid 1.0 / (1.0 - std::exp(-value))
	return tanh(value);
}

double Neuron::TransferFunctionDerv(double value)
{
	double tanh_value = tanh(value);
	return 1.0 - tanh_value * tanh_value;
}