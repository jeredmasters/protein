// http://www.opennn.net/documentation/templates.html


// System includes

#include <iostream>
#include <time.h>

// OpenNN includes

#include "../opennn/opennn.h"

using namespace OpenNN;

void main(){
	std::cout << "OpenNN. Genetic Robotics." << std::endl;

	std::cout << "Loading Data..." << std::endl;
	
	DataSet data_set;

	data_set.set_data_file_name("data/robotic_data.dat");

	data_set.load_data();

	data_set.set_file_type("dat");

	//  ####  INPUT SCALING
	std::cout << "Input scaling..." << std::endl;
	Instances* instances_pointer = data_set.get_instances_pointer();

	instances_pointer->split_random_indices();

	const Vector< Statistics<double> > inputs_statistics =
		data_set.scale_inputs_minimum_maximum();

	const Vector< Statistics<double> >
		targets_statistics = data_set.scale_targets_minimum_maximum();


	Variables* variables_pointer = data_set.get_variables_pointer();

	const size_t inputs_number = variables_pointer->count_inputs_number();
	const size_t outputs_number = variables_pointer->count_targets_number();
	std::cout << "Inputs: " << inputs_number << "Outputs: " << outputs_number << std::endl;
	//  ####  NEURAL NETWORK AND SCALING
	std::cout << "Creating network..." << std::endl;
	NeuralNetwork neural_network(inputs_number, 9, outputs_number);

	neural_network.construct_scaling_layer();

	ScalingLayer* scaling_layer_pointer = neural_network.get_scaling_layer_pointer();

	scaling_layer_pointer->set_statistics(inputs_statistics);

	scaling_layer_pointer->set_scaling_method(ScalingLayer::NoScaling);

	//  ####  LOSS INDEX
	std::cout << "Setting up loss index..." << std::endl;
	LossIndex loss_index(&neural_network, &data_set);

	loss_index.set_error_type("NORMALIZED_SQUARED_ERROR");
	loss_index.set_regularization_type(LossIndex::NEURAL_PARAMETERS_NORM);


	//  ####  TRAINING
	std::cout << "Setting up training..." << std::endl;
	TrainingStrategy training_strategy(&loss_index);

	training_strategy.set_main_type(TrainingStrategy::QUASI_NEWTON_METHOD);

	QuasiNewtonMethod* quasi_Newton_method_pointer =
		training_strategy.get_quasi_Newton_method_pointer();

	quasi_Newton_method_pointer->set_maximum_iterations_number(1000);
	quasi_Newton_method_pointer->set_display_period(10);
	std::cout << "Performing training..." << std::endl;
	training_strategy.perform_training();

	//  ####  TESTING
	std::cout << "Running testsg..." << std::endl;
	TestingAnalysis testing_analysis(&neural_network, &data_set);

	TestingAnalysis::LinearRegressionResults linear_regression_results =
		testing_analysis.perform_linear_regression_analysis();

	scaling_layer_pointer->set_scaling_method(ScalingLayer::MinimumMaximum);

	//  ####  SAVE RESULT

	neural_network.save_expression("expresion.txt");

	neural_network.save_expression_python("expresion.py");

	neural_network.save_expression_R("expresion.R");

}