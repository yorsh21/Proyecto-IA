#!/usr/bin/env python
# coding=utf-8

import math
import sys
import numpy as np
from pylab import *
import matplotlib.pyplot as plt
import os.path

num_trucks = 0
trucks_capacities = []
num_milks = 0
milk_request = []
milk_values = []
num_farms = 0
farms = []
letters = ["A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L"];

def convert_array(element):
	try:
	   return float(element)
	except ValueError:
		if(element in letters):
			return letters.index(element)
		else:
			return -1


def read_instances(instance):
	file = open(os.path.dirname(__file__) + "/../inputs/" + instance + ".txt")

	global num_trucks
	global trucks_capacities
	global num_milks
	global milk_request
	global milk_values
	global num_farms

	num_trucks = int(file.readline())
	trucks_capacities = file.readline()[:-1].split("\t")
	file.readline()

	num_milks = int(file.readline())
	milk_request = file.readline()[:-1].split("\t")
	milk_values = file.readline()[:-1].split("\t")
	file.readline()

	num_farms = int(file.readline())

	for farm in range(num_farms):
		line = file.readline()[:-1]
		array = list(map(convert_array, line.split("\t")))
		farms.append(array)


def plot_map(instance, route = None, output = ""):
	colors = ['c', 'm', 'y']
	plt.figure(num=None, figsize=(25, 25), dpi=200*0.4, facecolor='w', edgecolor='k')
	plt.xlabel('x')
	plt.ylabel('y')
	
	for f in farms:
	    if f[3] == -1:
	        scatter(f[1], f[2], s=60 ,marker='s', c='k')
	    elif f[3] == 0:
	        scatter(f[1], f[2], s=20 ,marker='o', c='r')
	    elif f[3] == 1:
	        scatter(f[1], f[2], s=20 ,marker='o', c='g')
	    elif f[3] == 2:
	        scatter(f[1],f[2], s=20 ,marker='o', c='b')
	    plt.text(f[1], f[2], f[0]-1, fontsize=10)
	
	plt.legend(('Planta', 'Granja con Leche A', 'Granja con Leche B', 'Granja con Leche C'))

	if route is not None:
		plt.title('Route: ' + route + ' ' + output)
		route_list = list(map(int, route[1:-1].split(",")))
		
		routex = [farms[route_list[0]][1]]
		routey = [farms[route_list[0]][2]]

		count_route = -1
		for i in range(1, len(route_list)):
			routex.append(farms[route_list[i]][1])
			routey.append(farms[route_list[i]][2])

			if route_list[i] == 0:
				plot(routex, routey, colors[count_route]+'-', linewidth = .5)
				count_route += 1
				routex = [farms[route_list[i]][1]]
				routey = [farms[route_list[i]][2]]

	plt.savefig(os.path.dirname(__file__) + "/../outputs/" + instance + "-" + output + ".png")


def analysis_instances(route):
	route_list = list(map(int, route[1:-1].split(",")))
	route_cost = []
	milk_income = [0]*num_milks
	collected_milk = [0]*num_milks

	temp_route = [0]
	temp_cost = 0
	temp_milk = 0
	temp_types = []
	real_types = []
	truck_index = 0
	cumulative_milk = 0
	
	for i in range(1, len(route_list)):
		if route_list[i] == 0:
			temp_route.append(route_list[i])
			if len(temp_types) != 0:
				collected_milk[temp_types[-1]] += temp_milk
			
			temp_cost += int(
				sqrt(
					(farms[route_list[i-1]][1] - farms[route_list[i]][1])**2 + 
					(farms[route_list[i-1]][2] - farms[route_list[i]][2])**2
				) + 0.5
			)

			#Capacidad de los camiones:
			if temp_milk > int(trucks_capacities[truck_index]):
				temp_cost += (temp_milk - int(trucks_capacities[truck_index]))*100

			route_cost.append(temp_cost)

			print(temp_route)
			#print(temp_types)
			print(real_types)
			print(
				"Costo Ruta: ", int(temp_cost), 
				"\tLeche Camion: ", temp_milk, "/", trucks_capacities[truck_index], "\n"
			)

			temp_route = [0]
			temp_cost = 0
			temp_milk = 0
			truck_index += 1
			cumulative_milk = 0
			temp_types = []
			real_types = []
		else:
			temp_cost += int(
				sqrt(
					(farms[route_list[i-1]][1] - farms[route_list[i]][1])**2 + 
					(farms[route_list[i-1]][2] - farms[route_list[i]][2])**2
				) + 0.5
			)

			milk_type = farms[route_list[i]][3]
			milk_income[milk_type] += farms[route_list[i]][4]
			temp_milk += farms[route_list[i]][4]
			temp_route.append(route_list[i])

			if milk_type >= cumulative_milk:
				cumulative_milk = milk_type
				temp_types.append(milk_type)
			else:
				temp_types.append(cumulative_milk)
			
			real_types.append(milk_type)

	#Solo impresión
	for i in range(num_milks):
		print("Requerimiento Planta: ", milk_request[i], "\tLeche recogida", collected_milk[i])
	print()

	
	#Mezcla de Leche en la Planta
	for i in reversed(range(1, num_milks)):
		if int(milk_request[i]) > collected_milk[i]:
			collected_milk[i-1] -= int(milk_request[i]) - collected_milk[i];
			collected_milk[i] = int(milk_request[i]);


	total_income = 0
	total_cost = int(sum(route_cost))
	for i in range(num_milks):
		print("Requerimiento Planta: ", milk_request[i], "\tLeche recogida", collected_milk[i])
		
		total_income += math.ceil(collected_milk[i]*float(milk_values[i]))
		if int(milk_request[i]) > collected_milk[i]:
			total_income -= int((int(milk_request[i]) - collected_milk[i])*float(milk_values[i]))*100

	print("\nIngresos Total: ", total_income, "\tCosto Total: ", total_cost, "\tBeneficio: ", total_income - total_cost)

	return total_income - total_cost


#Read Args
if len(sys.argv) == 1:
	print("It is need pass the instance name")

elif len(sys.argv) == 2:
	read_instances(sys.argv[1])
	plot_map(sys.argv[1])

elif len(sys.argv) == 3:
	read_instances(sys.argv[1])
	print()
	analysis_instances(sys.argv[2])

elif len(sys.argv) == 4:
	read_instances(sys.argv[1])
	trucks_capacities = list(map(int, sys.argv[3][1:-1].split(",")))

	print()
	quality = analysis_instances(sys.argv[2])

	plot_map(sys.argv[1], sys.argv[2], str(quality))


print()