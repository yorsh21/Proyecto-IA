#include "Solver.h"


Solver::Solver(vector<int> capacities, vector<float> values, vector<vector<int>> locates, vector<int> cuotes, string instance) {
	trucks_lenght = capacities.size();
	milks_lenght = values.size();
	farms_lenght = locates.size();
	plant_cuotes = cuotes;
	truck_capacities = capacities;
	milk_values = values;
	farms_locates = locates;
	name_instance = instance;
	global_quality = -9999999;

	for (int i = 0; i < farms_lenght; ++i)
	{
		vector<int> row(farms_lenght, 0);
		for (int j = 0; j < farms_lenght; ++j)
		{
			row[j] = (int)sqrt(
				pow(farms_locates[i][0] - farms_locates[j][0], 2) + 
				pow(farms_locates[i][1] - farms_locates[j][1], 2)
			) + 0.5;
		}
		cost_matrix.push_back(row);
	}
	//print_cost_matrix();
}

int Solver::evaluate_old(vector<int> solution, bool show = false) {
	int route_cost = 0;
	int total_cost = 0;
	int collect_milk = 0;
	int truck_index = 0;

	milks_trucks = truck_capacities;
	satisfied_cuotes = plant_cuotes;
	pivots = {};
	quality_by_route = {};
	onetype = {};

	int local_quality = farms_locates[solution[1]][2]; //Tipo de leche de la primera granja
	int len_sol = (int)solution.size();
	bool temp_ot = true;

	for (int i = 1; i < len_sol; ++i) {
		vector<int> current_farm = farms_locates[solution[i]];
		route_cost += cost_matrix[solution[i]][solution[i-1]];

		//El nodo actual es la planta
		if(solution[i] == 0) {
			total_cost += route_cost;
			milks_trucks[truck_index] -= collect_milk;
			satisfied_cuotes[local_quality] -= collect_milk;
			pivots.push_back(i);
			quality_by_route.push_back(local_quality);
			onetype.push_back(temp_ot);

			//Exceso en la capacidad de los camiones
			if (milks_trucks[truck_index] < 0) {
				total_cost -= milks_trucks[truck_index]*100;
			}

			if(i+1 < len_sol) {
				route_cost = 0;
				collect_milk = 0;
				truck_index++;
				local_quality = farms_locates[solution[i+1]][2];
				temp_ot = true;
			}
		}
		else {
			collect_milk += current_farm[3];

			if(milk_values[local_quality] > milk_values[current_farm[2]]) {
				local_quality = current_farm[2];
				temp_ot = false;
			}
		}
	}

	milk_income = 0;
	for (int i = 0; i < milks_lenght; ++i) {
		milk_income += (plant_cuotes[i] - satisfied_cuotes[i])*milk_values[i] + 0.5;
		
		//Penalización por cuota faltante
		if(satisfied_cuotes[i] >= 0) {
			milk_income -= satisfied_cuotes[i]*milk_values[i]*100;
		}
	}

	if (show) {
		cout << endl << "Total: " << milk_income << " - " << total_cost << " = " << milk_income - total_cost << endl << endl;

		cout << "Capacidad Camiones: ";
		print_int_vector(truck_capacities);
		cout << "Capacidad Restante: ";
		print_int_vector(milks_trucks);

		cout << endl;
		
		cout << "Cuotas Planta:    ";
		print_int_vector(plant_cuotes);
		cout << "Cuotas Faltantes: ";
		print_int_vector(satisfied_cuotes);

		cout << endl;
		//draw_graph(solution, milk_income - total_cost);
	}

	return milk_income - total_cost;
}

int Solver::evaluate(Solution *sol, bool show = false) {
	int route_cost = 0;
	int collect_milk = 0;
	int truck_index = 0;
	int local_quality = farms_locates[sol->route[1]][2]; //Tipo de leche de la primera granja
	bool temp_ot = true;

	sol->milks_trucks = truck_capacities;
	sol->satisfied_cuotes = plant_cuotes;

	for (int i = 1; i < sol->lenght; ++i) {
		vector<int> current_farm = farms_locates[sol->route[i]];
		route_cost += cost_matrix[sol->route[i]][sol->route[i-1]];

		//El nodo actual es la planta
		if(sol->route[i] == 0) {
			sol->total_cost += route_cost;
			sol->milks_trucks[truck_index] -= collect_milk;
			sol->satisfied_cuotes[local_quality] -= collect_milk;
			sol->pivots.push_back(i);
			sol->quality_by_route.push_back(local_quality);
			sol->onetype.push_back(temp_ot);

			//Exceso en la capacidad de los camiones
			if (sol->milks_trucks[truck_index] < 0) {
				sol->total_cost -= sol->milks_trucks[truck_index]*100;
			}

			if(i+1 < sol->lenght) {
				route_cost = 0;
				collect_milk = 0;
				truck_index++;
				local_quality = farms_locates[sol->route[i+1]][2];
				temp_ot = true;
			}
		}
		else {
			collect_milk += current_farm[3];

			if(milk_values[local_quality] > milk_values[current_farm[2]]) {
				local_quality = current_farm[2];
				temp_ot = false;
			}
		}
	}

	sol->total_benefit = 0;
	for (int i = 0; i < milks_lenght; ++i) {
		sol->total_benefit += (plant_cuotes[i] - sol->satisfied_cuotes[i])*milk_values[i] + 0.5;
		
		//Penalización por cuota faltante
		if(sol->satisfied_cuotes[i] >= 0) {
			sol->total_benefit -= sol->satisfied_cuotes[i]*milk_values[i]*100;
		}
	}

	if (show) {
		cout << endl << "Total: " << sol->total_benefit << " - " << sol->total_cost << " = " << sol->total_benefit - sol->total_cost << endl << endl;

		cout << "Capacidad Camiones: ";
		print_int_vector(truck_capacities);
		cout << "Capacidad Restante: ";
		print_int_vector(sol->milks_trucks);

		cout << endl;
		
		cout << "Cuotas Planta:    ";
		print_int_vector(plant_cuotes);
		cout << "Cuotas Faltantes: ";
		print_int_vector(sol->satisfied_cuotes);

		cout << endl;
		//draw_graph(sol->route, sol->total_benefit - sol->total_cost);
	}

	return sol->total_benefit - sol->total_cost;
}

int Solver::intra_evaluate(vector<int> solution, int old_eval, int index1, int index2) {
	int new_eval = old_eval;

	int k1 = min(index1, index2);
	int k2 = max(index1, index2);

	new_eval += cost_matrix[solution[k1]][solution[k1-1]];
	new_eval += cost_matrix[solution[k2]][solution[k2+1]];
	
	new_eval -= cost_matrix[solution[k2]][solution[k1-1]];
	new_eval -= cost_matrix[solution[k1]][solution[k2+1]];

	return new_eval;
}

int Solver::extra_evaluate(Solution sol, int index1, int index2) {
	//cout << ":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::" << endl;
	int g1 = -1;
	int g2 = -1;
	for (int i = 0; i < (int)sol.pivots.size(); ++i) 
	{
		if (g1 == -1 && index1 < sol.pivots[i]) g1 = i;
		if (g2 == -1 && index2 < sol.pivots[i]) g2 = i;
	}

	if (index1 < index2) {
		//Quitar Rutas
		sol.total_cost -= cost_matrix[sol.route[index1]][sol.route[index1-1]];
		sol.total_cost -= cost_matrix[sol.route[index1]][sol.route[index1+1]];
		sol.total_cost -= cost_matrix[sol.route[index2]][sol.route[index2+1]];
		
		//Agregar Rutas
		sol.total_cost += cost_matrix[sol.route[index1-1]][sol.route[index1+1]];
		sol.total_cost += cost_matrix[sol.route[index1]][sol.route[index2]];
		sol.total_cost += cost_matrix[sol.route[index1]][sol.route[index2+1]];
	}
	else if(index2 < index1) {
		//Quitar Rutas
		sol.total_cost -= cost_matrix[sol.route[index1]][sol.route[index1-1]];
		sol.total_cost -= cost_matrix[sol.route[index1]][sol.route[index1+1]];
		sol.total_cost -= cost_matrix[sol.route[index2]][sol.route[index2-1]];
		
		//Agregar Rutas
		sol.total_cost += cost_matrix[sol.route[index1-1]][sol.route[index1+1]];
		sol.total_cost += cost_matrix[sol.route[index1]][sol.route[index2-1]];
		sol.total_cost += cost_matrix[sol.route[index1]][sol.route[index2]];
	}

	//Quitando exceso en la capacidad de los camiones
	if (sol.milks_trucks[g1] < 0) {
		sol.total_cost += sol.milks_trucks[g1]*100;
	}
	if (sol.milks_trucks[g2] < 0) {
		sol.total_cost += sol.milks_trucks[g2]*100;
	}

	//Analizar al quitar nodo index1
	if (sol.onetype[g1]){
		sol.satisfied_cuotes[sol.quality_by_route[g1]] += farms_locates[sol.route[index1]][3];
	}
	else {
		int local_milk = 0;
		for (int i = sol.pivots[g1-1]+1; i < sol.pivots[g1]; ++i)
		{
			if (i != index1 && farms_locates[sol.route[index1]][2] > local_milk)
				local_milk = farms_locates[sol.route[index1]][2];
		}

		if (sol.quality_by_route[g1] > local_milk)
		{
			sol.satisfied_cuotes[sol.quality_by_route[g1]] += farms_locates[sol.route[index1]][3];
			sol.quality_by_route[g1] = local_milk;
		}
	}

	//Analizar al agregar nodo index2
	if (farms_locates[sol.route[index1]][2] <= sol.quality_by_route[g2]) {
		sol.satisfied_cuotes[sol.quality_by_route[g2]] -= farms_locates[sol.route[index1]][3];
	}
	else {
		sol.satisfied_cuotes[sol.quality_by_route[g1]] -= (truck_capacities[g2] - sol.milks_trucks[g2]);
		sol.satisfied_cuotes[sol.quality_by_route[g2]] += (truck_capacities[g2] - sol.milks_trucks[g2]);
		sol.satisfied_cuotes[sol.quality_by_route[g2]] -= farms_locates[sol.route[index1]][3];

		sol.quality_by_route[g2] = farms_locates[sol.route[index1]][2];
	}

	//Recalculando Cosotos
	sol.total_benefit = 0;
	for (int i = 0; i < milks_lenght; ++i) {
		sol.total_benefit += (plant_cuotes[i] - sol.satisfied_cuotes[i])*milk_values[i] + 0.5;
		
		//Penalización por cuota faltante
		if(sol.satisfied_cuotes[i] >= 0) {
			sol.total_benefit -= sol.satisfied_cuotes[i]*milk_values[i]*100;
		}
	}
	
	//cout << sol.total_benefit << " - " << sol.total_cost << " = " << sol.total_benefit - sol.total_cost << endl;
	//cout << ":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::" << endl;

	return sol.total_benefit - sol.total_cost;
}

int Solver::random_index(vector<int> solution) {
	int index = 0;
	while(solution[index] == 0) {
		index = rand() % (farms_lenght + trucks_lenght - 2) + 1;
	}

	return index;
}


/************************************************************/
/********************* Búsqueda Local  **********************/
/************************************************************/

vector<int> Solver::hill_climbing(int end_time) {
	clock_t begin = clock();
	int ciclos = 0;

	Solution best_solution = Solution(random_feasible_solution());
	int quality_best = evaluate(&best_solution);
	int index = 0;

	vector<int> neighbour;
	Solution new_neighbour = best_solution;

	//Loop restarts/time
	while (float(clock() - begin) / CLOCKS_PER_SEC < end_time) {
		Solution solution = Solution(random_feasible_solution());
		int quality = evaluate(&solution);
		int neighbour_quality = 0;
		int neighbour_size = 0;

		//Busqueda Local Intra Rutas
		bool local = false;
		while(!local) {
			local = true;
			index = solution.random_index();
			neighbour = solution.neighbour_2opt_index(index);
			neighbour_size = (int)neighbour.size();
			for (int i = 0; i < neighbour_size; ++i)
			{
				neighbour_quality = intra_evaluate(solution.route, quality, index, neighbour[i]);
				if(neighbour_quality > quality) {
					solution = two_opt(solution, index, neighbour[i]);
					quality = neighbour_quality;
					if(quality > quality_best) {
						global_trucks_position = truck_capacities;
						local = false;
					}
					break;
				}
			}
		}

		//quality = evaluate(&solution);

		//Busqueda Local Entre Rutas
		local = false;
		while(!local) {
			local = true;

			index = solution.random_index();
			neighbour = solution.neighbour_move_index(index);
			neighbour_size = (int)neighbour.size();
			for (int i = 0; i < neighbour_size; ++i)
			{
				new_neighbour = move_extra_routes(solution, index, neighbour[i]);
				//neighbour_quality = extra_evaluate(solution, index, neighbour[i]);
				neighbour_quality = evaluate(&new_neighbour);
				
				/*int neighbour_quality2 = evaluate(&new_neighbour);
				if(neighbour_quality != neighbour_quality2){
					print_int_vector(solution);
					print_int_vector(new_neighbour);
					cout << index << " | "  << neighbour[i] << endl;
					cout << neighbour_quality << endl;
					cout << neighbour_quality2 << endl;
					return solution;
				}*/

				if(neighbour_quality > quality) {
					solution = new_neighbour;
					quality = neighbour_quality;
					if(quality > quality_best) {
						global_trucks_position = truck_capacities;
						local = false;
					}
					break;
				}
			}
		}

		//quality = evaluate(&solution);

		//Busqueda Local Intra Rutas
		local = false;
		while(!local) {
			local = true;
			index = solution.random_index();
			neighbour = solution.neighbour_2opt_index(index);
			neighbour_size = (int)neighbour.size();
			for (int i = 0; i < neighbour_size; ++i)
			{
				neighbour_quality = intra_evaluate(solution.route, quality, index, neighbour[i]);
				if(neighbour_quality > quality) {
					solution = two_opt(solution, index, neighbour[i]);
					quality = neighbour_quality;
					if(quality > quality_best) {
						global_trucks_position = truck_capacities;
						local = false;
					}
					break;
				}
			}
		}


		if ((int)global_trucks_position.size() == trucks_lenght) {
			truck_capacities = global_trucks_position;
		}

		if(quality > quality_best) {
			best_solution = solution;
			quality_best = quality;

			cout << float(clock() - begin) / CLOCKS_PER_SEC << ": " << quality_best << endl;
			//print_int_vector(solution);
		}

		ciclos++;
	}

	if(quality_best > global_quality) {
		global_quality = quality_best;
		global_solution = best_solution.route;
	}

	clock_t end = clock();
	result_times.push_back(float(end - begin) / CLOCKS_PER_SEC);
	result_qualities.push_back(quality_best);

	return best_solution.route;
}


vector<int> Solver::hill_climbing_old(int end_time) {
	clock_t begin = clock();
	int ciclos = 0;

	vector<int> neighbour;
	vector<int> new_neighbour;
	vector<int> best_solution = random_feasible_solution();
	int quality_best = evaluate_old(best_solution);
	int index = 0;

	//Loop restarts/time
	while (float(clock() - begin) / CLOCKS_PER_SEC < end_time) {
		vector<int> solution = random_feasible_solution();
		int quality = evaluate_old(solution);
		int neighbour_quality = 0;
		int neighbour_size = 0;

		//Busqueda Local Intra Rutas
		bool local = false;
		while(!local) {
			local = true;

			index = random_index(solution);
			neighbour = neighbour_2opt_index(solution, index);
			neighbour_size = (int)neighbour.size();
			for (int i = 0; i < neighbour_size; ++i)
			{
				new_neighbour = two_opt_old(solution, index, neighbour[i]);
				neighbour_quality = intra_evaluate(solution, quality, index, neighbour[i]);
				//neighbour_quality = evaluate_old(new_neighbour);

				if(neighbour_quality > quality) {
					solution = new_neighbour;
					quality = neighbour_quality;
					if(quality > quality_best) {
						global_trucks_position = truck_capacities;
						local = false;
					}
					break;
				}
			}
		}

		if ((int)global_trucks_position.size() == trucks_lenght) {
			truck_capacities = global_trucks_position;
		}

		if(quality > quality_best) {
			best_solution = solution;
			quality_best = quality;

			cout << ciclos << ": " << quality_best << endl;
			//print_int_vector(solution);
		}

		ciclos++;
	}

	if(quality_best > global_quality) {
		global_quality = quality_best;
		global_solution = best_solution;
	}

	clock_t end = clock();
	result_times.push_back(float(end - begin) / CLOCKS_PER_SEC);
	result_qualities.push_back(quality_best);

	return best_solution;
}




vector<int> Solver::short_swap(vector<int> solution, int index) {
	if(index > 0 && index < (int)solution.size()-2) {
		int temp = solution[index];
		solution[index] = solution[index+1];
		solution[index+1] = temp;
	}

	return solution;
}

vector<int> Solver::long_swap(vector<int> solution, int index, int move) {
	if(move > 0 && move < (int)solution.size()-2) {
		int temp = solution[move];
		solution[move] = solution[index];
		solution[index] = temp;
	}

	return solution;
}

vector<int> Solver::two_opt_old(vector<int> solution, int index1, int index2) {
	int k1 = min(index1, index2);
	int k2 = max(index1, index2);

	int diff = (k2 - k1)/2;
	for (int i = 0; i <= diff; ++i)
	{
		int temp = solution[k1 + i];
		solution[k1 + i] = solution[k2 - i];
		solution[k2 - i] = temp;
	}

	return solution;
}

Solution Solver::two_opt(Solution solution, int index1, int index2) {
	Solution new_solution = solution;
	int k1 = min(index1, index2);
	int k2 = max(index1, index2);

	int diff = (k2 - k1)/2;
	for (int i = 0; i <= diff; ++i)
	{
		int temp = new_solution.route[k1 + i];
		new_solution.route[k1 + i] = new_solution.route[k2 - i];
		new_solution.route[k2 - i] = temp;
	}

	return new_solution;
}

vector<int> Solver::move_extra_routes_old(vector<int> solution, int index1, int index2) {
	if (index1 < index2) {
		int temp = solution[index1];
		for (int i = index1; i < index2; ++i) {
			solution[i] = solution[i+1];
		}
		solution[index2] = temp;
	}
	else if(index2 < index1) {
		int temp = solution[index1];
		for (int i = index1; i > index2; --i) {
			solution[i] = solution[i-1];
		}
		solution[index2] = temp;
	}

	return solution;
}

Solution Solver::move_extra_routes(Solution solution, int index1, int index2) {
	Solution new_solution = solution;
	if (index1 < index2) {
		int temp = new_solution.route[index1];
		for (int i = index1; i < index2; ++i) {
			new_solution.route[i] = new_solution.route[i+1];
		}
		new_solution.route[index2] = temp;
	}
	else if(index2 < index1) {
		int temp = new_solution.route[index1];
		for (int i = index1; i > index2; --i) {
			new_solution.route[i] = new_solution.route[i-1];
		}
		new_solution.route[index2] = temp;
	}

	return new_solution;
}

vector<int> Solver::neighbour_2opt_index(vector<int> solution, int index) {
	vector<int> indexes;
	int len = (int)solution.size();

	int count = 1;
	while(index + count < len && solution[index + count] != 0) {
		indexes.push_back(index + count);
		count++;
	}

	count = 1;
	while(index - count > 0 && solution[index - count] != 0) {
		indexes.push_back(index - count);
		count++;
	}

	return indexes;
}

vector<int> Solver::neighbour_move_index(vector<int> solution, int index) {
	vector<int> indexes;
	int len = (int)solution.size();

	int route1 = -1;
	int route2 = -1;
	for (int i = 0; i < len; ++i) {
		if (solution[i] == 0) {
			if (route1 != -1 && route2 == -1 && i >= index){
				route2 = i;
			}

			if (i < index){
				route1 = i;
			}
		}
	}

	for (int i = 1; i < route1; ++i) {
		if (solution[i] != 0) {
			indexes.push_back(i);
		}
	}

	for (int i = route2; i < len; ++i) {
		if (solution[i] != 0) {
			indexes.push_back(i);
		}
	}

	return indexes;
}

vector<int> Solver::random_feasible_solution() {
	//Desordenando camiones
	truck_capacities = random_assignment(truck_capacities);

	vector<int> milks;
	for (int i = 0; i < milks_lenght; i++) milks.push_back(i);
	
	int itetator = 1;
	vector<int> solution(farms_lenght + trucks_lenght, 0);
	for (int i = 0; i < trucks_lenght; ++i)
	{
		int init = rand() % farms_lenght;
		while (farms_locates[init][2] == -1 || find (milks.begin(), milks.end(), farms_locates[init][2]) == milks.end()) {
			init = rand() % farms_lenght;
		}
		milks[farms_locates[init][2]] = -1;
		
		int counter = 0;
		vector<int> r = random_int_vector(farms_lenght);
		for (int j = 0; j < farms_lenght; ++j)
		{
			if (farms_locates[init][2] == farms_locates[r[j]][2])
			{
				solution[itetator + counter] = r[j];
				counter++;
			}
		}
		itetator += counter + 1;
	}

	return solution;
}


vector<int> Solver::random_solution() {
	vector<int> solution(farms_lenght + trucks_lenght, 0);

	int index = 1;
	while(index < farms_lenght) {
		int i = rand() % (farms_lenght+1) + 1;
		
		if(solution[i] == 0) {
			solution[i] = index;
			index++;
		}
		else {
			continue;
		}
	}
	return solution;
}


vector<int> Solver::random_int_vector(int lenght) {
	vector<int> int_vector(lenght, 0);

	int index = 1;
	while(index < lenght) {
		int i = rand() % lenght;
		
		if(int_vector[i] == 0) {
			int_vector[i] = index;
			index++;
		}
	}
	return int_vector;
}

vector<int> Solver::random_assignment(vector<int> array) {
	int lenght = (int)array.size();
	vector<int> int_vector(lenght, 0);

	int index = 0;
	while(index < lenght) {
		int i = rand() % lenght;
		
		if(int_vector[i] == 0) {
			int_vector[i] = array[index];
			index++;
		}
	}
	return int_vector;
}


/************************************************************/
/************************ Utilities *************************/
/************************************************************/

void Solver::print_int_vector(vector<int> array) {
	cout << "[";
	for (int i = 0; i < (int)array.size() - 1; ++i)
	{
		cout << array[i] << ",";
	}
	if ((int)array.size() > 0) {
		cout << array[(int)array.size()-1] <<  "]" << endl;
	}
	else {
		cout << "]" << endl;
	}
}

void Solver::print_float_vector(vector<float> array) {
	cout << "[";
	for (int i = 0; i < (int)array.size() - 1; ++i)
	{
		cout << array[i] << ",";
	}
	cout << array[(int)array.size()-1] <<  "]" << endl;
}

void Solver::print_farms_locates() {
	for (int i = 0; i < (int)farms_locates.size(); ++i)
	{
		cout << farms_locates[i][0] << " - " <<  farms_locates[i][1] << " - "  <<  farms_locates[i][2] << " - " <<  farms_locates[i][3]  <<  endl;
	}
	cout << "Total elementos: " << farms_locates.size() << endl << endl;
}

void Solver::print_cost_matrix() {
	for (int i = 0; i < (int)cost_matrix.size(); ++i)
	{
		string row = "-";
		for (int j = 0; j < (int)cost_matrix.size(); ++j)
		{
			row += to_string((int)cost_matrix[i][j])  + "-";
		}
		cout << row <<  endl;
	}
	cout << endl;
}

string Solver::int_vector_to_string(vector<int> array) {
	int len = (int)array.size();
	if (len == 0) {
		return "[]";
	} 
	else {
		string output = "[";
		for (int i = 0; i < len-1; ++i) {
			output += to_string(array[i]) + ",";
		}
		output += to_string(array[len]) + "]";

		return output;
	}
}


/************************************************************/
/************************** Exports *************************/
/************************************************************/

void Solver::export_result(vector<int> solution) {
	string file = "outputs/" + name_instance + ".out";
	cout << "Writing output to: " << file << endl;

	ofstream myfile;
	myfile.open (file);

	if (myfile.is_open()) {
		float route_cost = 0.0;
		float milk_income = 0.0;
		int local_quality = 100;
		int collect_milk = 0.0;
		vector<int> milk_trunk(trucks_lenght, 0);
		vector<float> distance_truck;
		vector<string> letters = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L"};
		vector<string> output;
		string local_output = "0-";
		string full_output = "python3 plot.py " + name_instance + " [0";

		for (int i = 1; i < (int)solution.size(); ++i) {
			route_cost += sqrt(
				pow(farms_locates[solution[i]][0] - farms_locates[solution[i]][1], 2) + 
				pow(farms_locates[solution[i-1]][0] - farms_locates[solution[i-1]][1], 2)
			);

			if(solution[i] == 0) {
				local_output += to_string(solution[i]);
			}
			else{
				local_output += to_string(solution[i]) + "-";
			}
			full_output += "," + to_string(solution[i]);

			collect_milk += farms_locates[solution[i]][3];
			if(solution[i] != 0 && local_quality > milk_values[farms_locates[solution[i]][2]]) {
				local_quality = farms_locates[solution[i]][2];
			}

			if(solution[i] == 0) {
				local_output += "\t" + to_string(route_cost) + "\t" + to_string(collect_milk) + letters[local_quality];
				output.push_back(local_output);
				local_output = "0-";

				distance_truck.push_back(route_cost);
				milk_trunk[local_quality] = collect_milk;

				route_cost = 0.0;
				local_quality = 100;
				collect_milk = 0;
			}
		}

		full_output += "]";
		cout << full_output << endl;
		system(full_output.c_str());

		route_cost = 0;
		for (int i = 0; i < (int)distance_truck.size(); ++i) {
			route_cost += distance_truck[i];
			if (milk_trunk[i] > truck_capacities[i])
				route_cost += (milk_trunk[i] - truck_capacities[i])*10;
		}

		for (int i = 0; i < (int)milk_trunk.size(); ++i) {
			if(plant_cuotes[i] - milk_trunk[i] >= 0) {
				milk_income += milk_trunk[i]*milk_values[i];
				milk_income -= (plant_cuotes[i] - milk_trunk[i])*milk_values[i]*10;
			}
			else {
				milk_income += plant_cuotes[i]*milk_values[i];
			}
		}

		string head_output = to_string(milk_income - route_cost) + "\t" + to_string(route_cost) + "\t\t" + to_string(milk_income);
		myfile << head_output << endl;
		for (int i = 0; i < (int)output.size(); ++i) {
			myfile << output[i] << endl;
		}

		cout << "Successful file writing" << endl;
	}
	else {
		cout << "Error when writing the file:" << file << endl;
	}
	myfile.close();
	
}

void Solver::save_row_result() {
	string file = "outputs/results.out";
	ofstream myfile;
	myfile.open (file, std::fstream::app);

	int best = -9999999;
	int sum = 0;
	int times = 0;
	int len = (int)result_qualities.size();
	for (int i = 0; i < len; ++i)
	{
		sum += result_qualities[i];
		times += result_times[i];
		if(result_qualities[i] > best)
			best = result_qualities[i];
	}

	if (myfile.is_open()) {
		string output = name_instance + "\t" + 
			to_string((int)sum/len) + "\t" + 
			to_string((int)best) + "\t" + 
			to_string((int)times) + "\t" + 
			int_vector_to_string(global_solution);
		myfile << output << endl;
	}
	else {
		cout << "Error when writing the file:" << file << endl;
	}
	
	myfile.close();
	cout << "Successfully wrhite instance: " << name_instance << endl;
}

void Solver::draw_graph(vector<int> solution, int quality) {
	string full_output = "python3 plot.py " + name_instance + " [0";

	for (int i = 1; i < (int)solution.size(); ++i) {
		full_output += "," + to_string(solution[i]);
	}
	full_output += "] " + to_string(quality) + " [" + to_string(truck_capacities[0]);

	for (int i = 1; i < (int)truck_capacities.size(); ++i) {
		full_output += "," + to_string(truck_capacities[i]);
	}
	full_output += "]";

	cout << full_output << endl;
	system(full_output.c_str());
}

