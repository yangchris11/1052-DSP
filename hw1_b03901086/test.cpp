/*=====================================================/
/ FileName     [ test.cpp ] .                          / 
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
const int M = 5 ;

FILE *foutput ;

char inputs[50] ; // testing sequence

double delta[N][T] ;  

HMM hmm[M] ;

void initialProcess( int idx  ){
    // cerr << "start initializing idx = " << idx << "..." << endl ;
    for( int s = 0 ; s < N ; ++s ){
        for( int t = 0 ; t < T ; ++t )
            delta[s][t] = 0.0 ;
    }
} 

double getProb( int modelnum ){
    for( int s = 0 ; s < N ; ++s ){
        double _init = hmm[modelnum].initial[s] ;
        int tmp = inputs[0] - 'A' ;
        double _obsv = hmm[modelnum].observation[tmp][s] ;
        delta[s][0] = _init * _obsv ;
    }
    for( int t = 1 ; t < T ; ++t ){
        for( int s2 = 0 ; s2 < N ; ++s2 ){
            for( int s1 = 0 ; s1 < N ; ++s1 ){
                double _tran = hmm[modelnum].transition[s1][s2] ;
                double _prob = delta[s1][t-1]*_tran ;
                if( _prob > delta[s2][t] ) delta[s2][t] = _prob ;
            }
            int tmp = inputs[t] - 'A' ;
            double _obsv = hmm[modelnum].observation[tmp][s2] ;
            delta[s2][t] *= _obsv ;
        }
    }
    double _ans = 0.0 ;
    for( int s = 0 ; s < N ; ++s ){
        _ans = max( _ans , delta[s][T-1] ) ;
    }
    return _ans ;
}

// void printDelta(){
//     for( int s = 0 ; s < N ; ++s ){
//         for( int t = 0 ; t < T ; ++t )
//             cerr << delta[s][t] << " ";
//         cerr << endl ;
//     }
// }

void ViterbiAlgorithm(){

    int _bstModel = -666 ;
    double _bstProb = -666.0 ;

    for( int m = 0 ; m < M ; ++m ){
        initialProcess(m) ;
        double _prob = getProb(m) ;
        // printDelta() ;
        if( _prob > _bstProb ){
            _bstModel = m+1 ;
            _bstProb = _prob ;
        }
    }

    // cerr << "model_0" << _bstModel << " " << _bstProb << endl;
    fprintf( foutput , "model_0%d.txt %.6e\n" , _bstModel , _bstProb ) ;
}

int main( int argc , char *argv[] ){

    // argv[1] = model list
    string modelList = argv[1] ;
    ifstream fileModelList(argv[1]) ;
    for( int i = 0 ; i < M ; ++i ){
        char modelName[50] ;
        fileModelList >> modelName ;
        // cerr << modelName << endl ;
        loadHMM( &hmm[i] , modelName ) ;
    }

    int line_count ;
    ifstream test_model(argv[2]) ;
	test_model.unsetf( ios_base::skipws ) ;
	line_count = count(istream_iterator<char>(test_model),istream_iterator<char>(),'\n');

    // cerr << line_count << endl ;

    foutput = open_or_die( argv[3] , "w" ) ;

    ifstream testinput( argv[2] ) ;

    for( int i = 0 ; i < line_count ; ++i ){
        for( int j = 0 ; j < T ; ++j ){
            testinput >> inputs[j] ; 
        }
        ViterbiAlgorithm() ;
    }

    return 0 ;
}