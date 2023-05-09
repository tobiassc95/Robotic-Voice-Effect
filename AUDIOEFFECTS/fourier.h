#ifndef FFT_H
#define FFT_H

#include <complex>
#define PI (float)3.141592653

class Fourier {
	unsigned int bit_rev(unsigned int num, unsigned int nbits); //algoritmo bit reverse.
	void swap(std::complex<float>* vect, unsigned int x, unsigned int y);
	std::complex<float> W[2048];

	unsigned int N; //cantidad de puntos.
	unsigned int S, G, B;

public:
	Fourier();
	void FFT(std::complex<float>* in, std::complex<float>* out, unsigned int N);
	void IFFT(std::complex<float>* in, std::complex<float>* out, unsigned int N);
};

#endif //FFT_H
