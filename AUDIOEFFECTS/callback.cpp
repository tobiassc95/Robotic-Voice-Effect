#include "callback.h"

DATA::DATA(unsigned int N_samples) {
	if (N_samples != 0) {
		samples = (float*) new float[N_samples]; //este sera nuestro arreglo gigante con todas las muestras.
		for (unsigned int i = 0; i < N_samples; i++) {
			samples[i] = 0;
		}
	}
	this->N_samples = N_samples; //el numero total de muestras (frames) para grabar.
}

void DATA::create_array(unsigned int N_samples) {
	if (N_samples != 0) {
		samples = (float*) new float[N_samples]; //este sera nuestro arreglo gigante con todas las muestras.
		for (unsigned int i = 0; i < N_samples; i++) {
			samples[i] = 0;
		}
	}
	this->N_samples = N_samples; //el numero total de muestras (frames) para grabar.
}

DATA::~DATA() {
	delete[] samples;
}

///////////////////////////////////////////////////////////////

int record_cb(const void *input, void *output, unsigned long buff_size, const PaStreamCallbackTimeInfo *time_info, PaStreamCallbackFlags status_flags, void *user_data) {
	DATA *data = (DATA*)user_data;
	const float *in = (const float*)input; //obtenemos el puntero al primer elemento del buffer con los datos de la entrada.
	int result; //sera el resultado del callback.

	if (input == NULL) //si no hay nada en el input, no hay nada para hacer, nos vamos.
		return paComplete;

	for (unsigned int i = data->item; i < (data->item + buff_size); i++) {
		if (i < data->N_samples)
			data->samples[i] = in[i - data->item]; //llenamos el arreglo de datos con los datos.
		else
			break;
	}

	if ((data->N_samples - data->item) < buff_size) { //si la cantidad de datos que faltan es menor que el buffer, entonces llegamos a los ultimos datos.
		data->item += (data->N_samples - data->item);
		result = paComplete;
	}
	else { //caso contrario, continua.
		data->item += buff_size;
		result = paContinue;
	}

	return result;
}

int playback_cb(const void *input, void *output, unsigned long buff_size, const PaStreamCallbackTimeInfo *time_info, PaStreamCallbackFlags status_flags, void *user_data) {
	DATA *data = (DATA*)user_data;
	float *out = (float*)output; //obtenemos el puntero al primer elemento del buffer con los datos de la salida.
	int result;

	if (data->samples == NULL) //si no hay nada el arreglo de datos, no hay nada para hacer, nos vamos.
		return paComplete;

	for (unsigned int i = data->item; i < (data->item + buff_size); i++) {
		if (i < data->N_samples)
			out[i - data->item] = data->samples[i]; //le vamos dando a la salida los datos.
		else
			out[i - data->item] = 0; //llenamos lo que falta del buffer con ceros.
	}

	if ((data->N_samples - data->item) < buff_size) { //si la cantidad de datos que faltan es menor que el buffer, entonces llegamos a los ultimos datos.
		data->item += (data->N_samples - data->item);
		result = paComplete;
	}
	else { //caso contrario, continua.
		data->item += buff_size;
		result = paContinue;
	}

	return result;
}