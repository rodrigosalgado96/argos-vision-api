# argos-vision-api
  This is my Final Paper for my bachelor's degree in control and automation engineering. It was published by a national magazine in the following link:
https://revistaft.com.br/argos-reconhecimento-e-validacao-do-uso-de-equipamento-de-protecao-individual-por-camera-e-google-vision/

## About
  In this projetc/article, we use IoT (Internet of Things) to demonstrate anddescribe all the steps and processes of a device that automates the verification ofthe use of PPE (Personal Protective Equipment) by the employee. The device isstructured in three stages: The physical part, which consists of themicrocontroller and its algorithm. The local server, which will receive theinformation sent by the microcontroller and communicate with the onlineserver, sending the processed response to the microcontroller. The cloud server,AutoML Vision, which will identify the elements present in the image and returnthe response tags according to training.

## Hardware
  A regular computer running the Server and an ESP32CAM (a microcontroller, from Espressif Sistems);

## Software
  This repository has all algorithyns used in this projetc.

### esp_argos.ino > ESP32CAM Algorithm
  Writen in C/C++, this one is a setup for wifi connection, communication with the local api and camera's setup and relative functions

### AutoML.js and Server.js
  They are the local APIs, the main functionality of each one is to receive a photo from the microcontroller, make a request to AutoML Vision* (a API from Google Inc) and receive an answer with an Object who contains the information about the picture.

### AutoML.js 
This is an example to communicate and receive information about everything in a photo, it communicates with the regular API from AutoML Vision.
### Server.js 
This is the local api server used in this project, because it communicates with the customized API from AutoML Vision, returning only the information about PPE.


#### AutoML Vision documentation
https://cloud.google.com/vision/automl/docs?_ga=2.25398772.-892150434.1663112927

## Authors
William Raulinavixius

Rodrigo J A Salgado

Katelin Ribeiro

Brendha Sales Bazoni

Vinicius Costa

Carlos Noriega
