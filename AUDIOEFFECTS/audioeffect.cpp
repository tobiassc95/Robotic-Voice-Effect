#include "audioeffect.h"
#include <iostream>

void Robotization::effect(float * samples, unsigned int N_samples) {
	data = (std::complex<float> *) new std::complex<float>[N_samples];
	for (unsigned int i = 0; i < N_samples; i++) {
		data[i] = {0, 0};
	}
	for (unsigned int i = 0; i < ((unsigned int)(N_samples*2/F_BUFF_SIZ) - 1); i++) {
		for (unsigned int j = 0; j < F_BUFF_SIZ; j++) {
			buffer[j] = { samples[j + i*F_BUFF_SIZ / 2], 0 }; //guardamos los valores en el buffer, el cual se le aplicará la FFT y la IFFT.
			buffer[j] = { buffer[j].real()*powf(sinf(j*PI / (F_BUFF_SIZ - 1)), 2), buffer[j].imag() }; //modificamos los datos con la ventana de Hann = (sin(n*PI/(N-1)))^2.
		} //La ventana de Hann tiene la ventaja de que es una ventana complementaria si hay un overlap del 50%, eso evita que la señal se distorsione en el tiempo.
		F.FFT(buffer, buffer, F_BUFF_SIZ); //le aplicamos la transformada de fourier.
		for (unsigned int j = 0; j < F_BUFF_SIZ; j++) {
			buffer[j] = { std::abs(buffer[j]), 0 }; //forzamos la fase a cero.
		}
		F.IFFT(buffer, buffer, F_BUFF_SIZ); //le aplicamos la antitransformada de fourier.
		for (unsigned int j = 0; j < F_BUFF_SIZ; j++) {
			data[j + i*F_BUFF_SIZ / 2] += buffer[j]; //se lo sumamos al arreglo de datos complejos.
		}
	}
	for (unsigned int i = 0; i < N_samples; i++) {
		samples[i] = data[i].real();
	}
}
