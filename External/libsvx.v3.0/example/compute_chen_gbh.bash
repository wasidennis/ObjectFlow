#!/bin/bash

mkdir chen_gbh
mkdir chen_gbh/bus
mkdir chen_gbh/container
mkdir chen_gbh/garden
mkdir chen_gbh/ice
mkdir chen_gbh/paris
mkdir chen_gbh/salesman
mkdir chen_gbh/soccer
mkdir chen_gbh/stefan

../gbh/gbh 5 500 200 0.5 20 ../svxbench/dataset/Chen_ppm/bus ./chen_gbh/bus
../gbh/gbh 5 500 200 0.5 20 ../svxbench/dataset/Chen_ppm/container ./chen_gbh/container
../gbh/gbh 5 500 200 0.5 20 ../svxbench/dataset/Chen_ppm/garden ./chen_gbh/garden
../gbh/gbh 5 500 200 0.5 20 ../svxbench/dataset/Chen_ppm/ice ./chen_gbh/ice
../gbh/gbh 5 500 200 0.5 20 ../svxbench/dataset/Chen_ppm/paris ./chen_gbh/paris
../gbh/gbh 5 500 200 0.5 20 ../svxbench/dataset/Chen_ppm/salesman ./chen_gbh/salesman
../gbh/gbh 5 500 200 0.5 20 ../svxbench/dataset/Chen_ppm/soccer ./chen_gbh/soccer
../gbh/gbh 5 500 200 0.5 20 ../svxbench/dataset/Chen_ppm/stefan ./chen_gbh/stefan

rm chen_gbh/bus/time.txt
rm chen_gbh/container/time.txt
rm chen_gbh/garden/time.txt
rm chen_gbh/ice/time.txt
rm chen_gbh/paris/time.txt
rm chen_gbh/salesman/time.txt
rm chen_gbh/soccer/time.txt
rm chen_gbh/stefan/time.txt
