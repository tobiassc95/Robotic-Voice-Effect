#include "fourier.h"
#include <iostream>

Fourier::Fourier() {
	float num;
	for (int i = 0; i < 2048; i++) {
		num = 2 * PI*bit_rev(i, 11) / 4096;
		W[i] = { cosf(num), -sinf(num) };
	}
}

unsigned int Fourier::bit_rev(unsigned int num, unsigned int nbits) {
	unsigned int rnum = num; //el numero a aplicarle el bit reverse.
	unsigned int count = nbits - 1;
	unsigned int mask; //La mascara limita los bits en el int.

	//Inicializo la mascara para limitar los bits en el int.
	switch (nbits) {
	case 1:
		mask = 1;
		break;
	case 2:
		mask = 3;
		break;
	case 3:
		mask = 7;
		break;
	case 4:
		mask = 15;
		break;
	case 5:
		mask = 31;
		break;
	case 6:
		mask = 63;
		break;
	case 7:
		mask = 127;
		break;
	case 8:
		mask = 255;
		break;
	case 9:
		mask = 511;
		break;
	case 10:
		mask = 1023;
		break;
	case 11:
		mask = 2047;
		break;
	case 12:
		mask = 4095;
		break;
	default:
		std::cerr << "Error in Bit Reverse function." << std::endl;
		return num;
		break;
	}

	//Algoritmo bit reverse.								//ejemplo: num = rnum = 1100 -> mask = 15 = 1111 -> count = 3.
	num >>= 1; //shift (num = num >> 1).					//num = 0110.
	while (num) {											//while (0110)	//while (0011)	//while (0001)
		rnum <<= 1; //shift (rnum = rnum << 1).				//rnum = 1000.	//rnum = 0000.	//rnum = 0010.
		rnum &= mask; //and (rnum = rnum & mask)			//rnum = 1000.	//rnum = 0000.	//rnum = 0010.
		rnum |= (num & 1); //or (rnum = rnum | (num & 1))	//rnum = 1000	//rnum = 0001.	//rnum = 0011.
			num >>= 1;										//num = 0011.	//num = 0001.	//num = 0000.
		count--;											//count = 2.	//count = 1.	//count = 0.
	}
	rnum <<= count;											//rnum = 0011.
	rnum &= mask;											//rnum = 0011.
	return rnum;
}

void Fourier::swap(std::complex<float>* vect, unsigned int x, unsigned int y) {
	std::complex<float> temp;
	temp = vect[x];
	vect[x] = vect[y];
	vect[y] = temp;
}

void Fourier::FFT(std::complex<float>* in, std::complex<float>* out, unsigned int N) {
	S = (unsigned int)log2(N); //Siendo S (stage) la cantidad de etapas del algoritmo.
	G = 1; //siendo G (group) la cantidad de grupos en la etapa actual (en este caso, la etapa 1).
	B = N / 2; //siendo B (butterfly) la cantidad de mariposas en el grupo actual (en este caso, el grupo 1).
	std::complex<float> Y, Z;

	if (in != out) {
		for (unsigned int i = 0; i < N; i++) {
			out[i] = in[i];
		}
	}
	if (N == 1)
		return;

	for (unsigned int s = 0; s < S; s++) {
		for (unsigned int g = 0; g < G; g++) {
			for (unsigned int b = 0; b < B; b++) {
				Y = out[b + 2*B*g];
				Z = out[b + 2*B*g + N/(unsigned int)pow(2, s + 1)]*W[g]; //arreglar.

				out[b + 2*B*g] = Y + Z;
				out[b + 2*B*g + N/(unsigned int)pow(2, s + 1)] = Y - Z;
			}
		}
		//Al seguir a la etapa siguiente, habran el doble de grupos (en la etapa) y la mitad de mariposas (por grupo).
		G = G * 2;
		B = B / 2;
	}

	//se acomoda el arreglo de salida con bit reverse.
	for (unsigned int n = 0, nrev; n < N; n++) {
		nrev = bit_rev(n, S);
		if (n < nrev) //para que no haga swap por segunda vez y para no swapear en un mismo lugar.
			swap(out, n, nrev);
	}
}

void Fourier::IFFT(std::complex<float>* in, std::complex<float>* out, unsigned int N) {
	S = (unsigned int)log2(N); //Siendo S (stage) la cantidad de etapas del algoritmo.
	G = 1; //siendo G (group) la cantidad de grupos en la etapa actual (en este caso, la etapa 1).
	B = N / 2; //siendo B (butterfly) la cantidad de mariposas en el grupo actual (en este caso, el grupo 1).
	std::complex<float> Y, Z;

	if (in != out) {
		for (unsigned int i = 0; i < N; i++) {
			out[i] = in[i]; //copiamos el contenido del arreglo de la entrada a la salida.
		}
	}
	if (N == 1)
		return;

	for (unsigned int s = 0; s < S; s++) {
		for (unsigned int g = 0; g < G; g++) {
			for (unsigned int b = 0; b < B; b++) {
				Y = out[b + 2 * B*g];
				Z = out[b + 2 * B*g + N / (unsigned int)pow(2, s + 1)] * std::conj(W[g]); //arreglar.

				out[b + 2 * B*g] = Y + Z;
				out[b + 2 * B*g + N / (unsigned int)pow(2, s + 1)] = Y - Z;
			}
		}
		//Al seguir a la etapa siguiente, habran el doble de grupos (en la etapa) y la mitad de mariposas (por grupo).
		G = G * 2;
		B = B / 2;
	}

	//se acomoda el arreglo de salida con bit reverse.
	for (unsigned int n = 0, nrev; n < N; n++) {
		nrev = bit_rev(n, S);
		if (n < nrev) //para que no haga swap por segunda vez y para no swapear en un mismo lugar.
			swap(out, n, nrev);
	}
	for (unsigned int i = 0; i < N; i++) {
		out[i] = out[i] / (std::complex<float>)N; //se le divide el numero de puntos como en la IDFT.
	}
}