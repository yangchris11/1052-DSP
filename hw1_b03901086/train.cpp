/*=====================================================/
/ FileName     [ train.cpp ]                           / 
/ Author       [ Cheng-Yen Yang (b03901086)]           /
/ Instructor   [ Professor Lin-Shan Lee ]              / 
/ Copyright    [ Copyleft(c), NTUEE , Taiwan ]         /
/=====================================================*/

#include <cassert>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <iterator>
#include <cstring>
#include <vector>
#include <algorithm>
#include "hmm.h"

using namespace std ;

const int N = 6 ;
const int T = 50 ;

char inputs[10000][50] ;

double alpha[N][T] ;
double beta[N][T] ;
double gammaV[N][T] ;
double gammaAll[N][T] ;
double gammaSum[N][T] ;
double epsilon[N][N][T] ;
double epsilonAll[N][N][T] ;

double init[N] ;
double tran[N][N] ;
double obsv[N][N] ;

int line_count ;

HMM hmm ;

void getAlpha( int num ){
	for( int t = 0 ; t < T ; ++t ){
		for( int s = 0 ; s < N ; ++s ){
			if( t == 0 ){
				int tmp = inputs[num][t] - 'A' ;
				alpha[s][t] = init[s] * obsv[tmp][s] ;
			}
			else{
				alpha[s][t] = 0.0 ;
				int tmp = inputs[num][t] - 'A' ;
				for( int i = 0 ; i < N ; ++i ){
					alpha[s][t] += alpha[i][t-1]*tran[i][s] ;
				}
				alpha[s][t] *= obsv[tmp][s] ;
			}
		}
	}
}

void getBeta( int num ){
	for( int t = T-1 ; t >= 0 ; --t ){
		for( int s = 0 ; s < N ; ++s ){
			if( t == T-1 ){
				beta[s][t] = 1.0 ;
			}
			else{
				beta[s][t] = 0.0 ;
				for( int i = 0 ; i < N ; ++i ){
					int tmp = inputs[num][t+1] - 'A' ;
					beta[s][t] += obsv[tmp][i]*tran[s][i]*beta[i][t+1] ;
				}
			}
		}
	} 
}

void getGamma( int num ){
	for( int t = 0 ; t < T ; ++t ){
		double sum = 0.0 ;
		for( int s = 0 ; s < N ; ++s ){
			sum += alpha[s][t]*beta[s][t] ;
		}
		assert( sum != 0 ) ;
		for( int s = 0 ; s < N ; ++s ){
			gammaV[s][t] = alpha[s][t]*beta[s][t] / sum ;
			int tmp = inputs[num][t] - 'A' ;
			gammaSum[tmp][s] += gammaV[s][t] ;
			gammaAll[s][t] += gammaV[s][t] ;
		}
	}
}

void getEpsilon( int num ){
	for( int t = 0 ; t < T-1 ; ++t ){
		double sum = 0.0 ;
		for( int s = 0 ; s < N ; ++s ){
			for( int i = 0 ; i < N ; ++i ){
				int tmp = inputs[num][t+1] - 'A' ; 
				sum += alpha[s][t]*tran[s][i]*obsv[tmp][i]*beta[i][t+1] ;
			}
		}
		assert( sum != 0 ) ;
		for( int s1 = 0 ; s1 < N ; ++s1 ){
			for( int s2 = 0 ; s2 < N ; ++s2 ){
				int tmp = inputs[num][t+1] - 'A' ;
				epsilon[s1][s2][t] = alpha[s1][t]*tran[s1][s2]*obsv[tmp][s2]*beta[s2][t+1] / sum ;
				epsilonAll[s1][s2][t] += epsilon[s1][s2][t] ;
			}
		}
	}
}


// void printAlpha(){
// 	cout << "========Alpha========" << endl ;
// 	for( int i = 0 ; i < N ; ++i ){
// 		for( int j = 0 ; j < 10 ; ++j ){
// 			cout << alpha[i][j] << " " ;
// 		}
// 		cout << endl ;
// 	}
// }

// void printBeta(){
// 	cout << "========Beta========" << endl ;
// 	for( int i = 0 ; i < N ; ++i ){
// 		for( int j = 0 ; j < T ; ++j ){
// 			cout << beta[i][j] << " " ;
// 		}
// 		cout << endl ;
// 	}
// }

// void printEpsilon(){
// 	cout << "========Epsilon========" << endl ;
// 	for( int i = 0 ; i < N ; ++i ){
// 		for( int j = 0 ; j < N ; ++j ){
// 			for( int k = 0 ; k < T ; ++k )
// 				cout << epsilon[i][j][k] << " " ;
// 		}
// 		cout << endl ;
// 	}
// }

void renewProcess(){
	for( int s = 0 ; s < N ; ++s ){
		init[s] = gammaAll[s][0] / line_count ;
	}
	for( int s = 0 ; s < N ; ++s ){
		double tmpgammaSum = 0.0 ;
		for( int t = 0 ; t < T-1 ; ++t ){
			tmpgammaSum += gammaAll[s][t] ;
		}
		for( int s2 = 0 ; s2 < N ; ++s2 ){
			double sum = 0.0 ;
			for( int t = 0 ; t < T-1 ; ++t ){
				sum += epsilonAll[s][s2][t] ;
			}
			tran[s][s2] = sum/tmpgammaSum ;
		}
		tmpgammaSum += gammaAll[s][T-1] ;
		double tmpSum[N] ;
		fill( tmpSum , tmpSum+N , 0 ) ;
		for( int i = 0 ; i < N ; ++i ){
			tmpSum[i] += gammaSum[i][s] ;
		}
		for( int i = 0 ; i < N ; ++i ){
			obsv[i][s] = tmpSum[i]/tmpgammaSum ;
		}
	}
}

void initialProcess(){
	//cerr << "start initializing..." << endl ;
	for( int i = 0 ; i < N ; ++i ){
		init[i] = hmm.initial[i] ;
		for( int j = 0 ; j < N ; ++j ){
			tran[i][j] = hmm.transition[i][j] ; 
			obsv[i][j] = hmm.observation[i][j] ; 
		}
	}
	for( int i = 0 ; i < N ; ++i ){
		fill( gammaAll[i] , gammaAll[i]+T , 0.0 ) ;
		fill( gammaSum[i] , gammaSum[i]+T , 0.0 ) ;
		for( int j = 0 ; j < N ; ++j ){
			fill( epsilonAll[i][j] , epsilonAll[i][j]+T , 0 ) ;
		}
	}
}

void finalizeProcess(){
	for( int i = 0 ; i < N ; ++i ){
		hmm.initial[i] = init[i] ;
		for( int j = 0 ; j < N ; ++j ){
			hmm.transition[i][j] = tran[i][j] ;
			hmm.observation[i][j] = obsv[i][j] ;
		}
	}
}

void getCoeffient( const int& num ){
	getAlpha(num) ;
	getBeta(num) ;
	getGamma(num) ;
	getEpsilon(num) ;
}

// void printTran(){
// 	for( int i = 0 ; i < N ; ++i ){
// 		for( int j = 0 ; j < N ; ++j ) cerr << tran[i][j] ;
// 		cerr << endl ;
// 	}
// }

void trainingProcess( const int& iter ){
	// cerr << "start training..." << endl ;
	for( int n = 0 ; n < iter ; ++n ){
		// cerr << "iter: " << n << endl ;
		for( int l = 0 ; l < line_count ; ++l ) getCoeffient(l) ;
		renewProcess() ;
		for( int i = 0 ; i < N ; ++i ){
			fill( gammaAll[i] , gammaAll[i]+T , 0.0 ) ;
			fill( gammaSum[i] , gammaSum[i]+T , 0.0 ) ;
			for( int j = 0 ; j < N ; ++j ){
				fill( epsilonAll[i][j] , epsilonAll[i][j]+T , 0 ) ;
			}
		}
	}
}

int main( int argc , char *argv[] ){

	// argv[1] = iteration time
	// argv[2] = init model 
	// argv[3] = input model 
	// argv[4] = output model
	// ex : ./train  iteration  model_init.txt  seq_model_01.txt  model_01.txt

	int iter = atoi(argv[1]) ;

	// cout << sys_warning << "iteration time = " << argv[1] << endl ;
	// cout << sys_warning << "file to train..." << argv[0] << endl ;

	loadHMM( &hmm , argv[2] ) ;

	ifstream seq_model(argv[3]) ;
	seq_model.unsetf( ios_base::skipws ) ;
	line_count = count(istream_iterator<char>(seq_model),istream_iterator<char>(),'\n');

	initialProcess() ;

	ifstream seq_test(argv[3]) ;
	for( int i = 0 ; i < line_count ; ++i ){
		for( int j = 0 ; j < 50 ; ++j ){
			seq_test >> inputs[i][j] ;
		}
	}

	trainingProcess(iter) ;
	
	finalizeProcess() ;
	FILE* fmod = fopen( argv[4] , "w" ) ;
	dumpHMM( fmod , &hmm ) ;

	return 0 ;
}

