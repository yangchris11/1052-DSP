#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <set>
#include <map>
#include <deque>
#include <stack>
#include <queue>
#include <algorithm>
#include "Ngram.h"

using namespace std;

#define NN 4096

string _replaceM[NN][NN] ;
short _backIdx[NN][NN];
double _delta[NN][NN] ;

vector<string> _map[256][256] ;

int main( int argc , char *argv[] ){
    
    int buf = 2048 ;

     Vocab _vocab ; 
     int _ngramOrder = (argv[8][0]-'0')%48 ;
     Ngram lm( _vocab , _ngramOrder ) ;

    char* _textFILE = argv[2] ;
    FILE * fin = fopen( _textFILE , "r" ) ;
    char* _mapFILE = argv[4] ;
    FILE * fmap = fopen( _mapFILE , "r" ) ;
    char* _lmFILE = argv[6] ;
    File lmFile( _lmFILE , "r" ) ;
    lm.read(lmFile) ;
    lmFile.close() ;

    bool _readMap = true ;

    while( _readMap ){

        int _zhu1 = fgetc( fmap ) ;   // _zhu1 : ZhuYin[0]
        int _zhu2 = fgetc( fmap ) ;   // _zhu2 : ZhuYin[1]
        bool _getMap = true ;

        if( _zhu1 == -1 || _zhu2 == -1 ) break ;
        while( _getMap ){
            
            char _tmp = fgetc( fmap ) ; 

            if( _tmp == ' ' ){
                continue ; // get next char
            }
            if( _tmp == '\n' ){
                break ; // get next map index
            }

            char _tmp2 = fgetc( fmap ) ;
            char _ZhuYinIdx[3] ;
            _ZhuYinIdx[0] = _tmp ;
            _ZhuYinIdx[1] = _tmp2 ;
            _ZhuYinIdx[2] = '\0' ;

            _map[_zhu1][_zhu2].push_back( _ZhuYinIdx ) ;

            // cerr << "Constructed?" << endl ;
            // cerr << "ZhuYin" << _ZhuYinIdx << endl ;
            // cerr << "(" << _zhu1  << "," << _zhu2 << ") = " ;
            // cerr << _map[_zhu1][_zhu2][_map[_zhu1][_zhu2].size() -1] << endl ;

        }

    }   

    VocabIndex _headIdx = _vocab.ssIndex() ;
    VocabIndex _endIdx  = _vocab.seIndex() ;
    VocabIndex context[2] = { 0 , Vocab_None } ;
    VocabIndex _tmpIdx[4][buf] ;  //possible  answer

    bool _readText = true ;

    while( _readText ){

        short _in1 = fgetc( fin ) ;
        if( _in1 == ' ' ) continue ;
        if( _in1 == -1 ) break ;
        short _in2 = fgetc( fin ) ;
        double _allProb = LogP_Zero ;
        short _allIdx = 0 ;

        // char _ZhuYinIdx[3] ;
        // _ZhuYinIdx[0] = _in1;
        // _ZhuYinIdx[1] = _in2 ;
        // _ZhuYinIdx[2] = '\0' ; 
        // cerr << "text: " <<  _ZhuYinIdx << endl ;

        int _mapSize = _map[_in1][_in2].size() ;  // number of possible mapping

        // cerr << "num of possible char: " <<  _mapSize << endl ;

        context[0] = _headIdx ;

        for( int i = 0 ; i < _mapSize ; ++i ){
            VocabIndex wid = _vocab.getIndex( _map[_in1][_in2][i].c_str() ) ;
            if( wid == Vocab_None ) wid = _vocab.unkIndex() ;
            _tmpIdx[0][i] = wid ;
            _delta[i][0] = lm.wordProb( wid , context ) ;    
            // cerr << "prob:" << _delta[i][0] << endl ;        
        }

        int t = 0 ;
        bool _getText = true ;

        while( _getText ){
            short _ans1 = fgetc( fin ) ;
            if( _ans1 == ' ' ) continue ;
            if( _ans1 == '\n' ) break ;
            short _ans2 = fgetc( fin ) ;
            ++t ;
            char _xZhuYinIdx[3] ;
            _xZhuYinIdx[0] = _ans1 ;
            _xZhuYinIdx[1] = _ans2 ;
            _xZhuYinIdx[2] = '\0' ; 
            int _ansmapSize = _map[_ans1][_ans2].size() ;
            // cerr << "gettext: " <<  _xZhuYinIdx << "possible" << _ansmapSize << endl ;
            for( int i = 0 ; i < _ansmapSize ; ++i ){

                _tmpIdx[1][i] = _vocab.getIndex( _map[_ans1][_ans2][i].c_str() ) ;
                if( _tmpIdx[1][i] == Vocab_None ) _tmpIdx[1][i] = _vocab.unkIndex() ;
                double _curProb = LogP_Zero ;
                int _replaceIdx = 0 ;

                for( int j = 0 ; j < _mapSize ; ++j ){ // get most possible CHI-char
                    context[0] = _tmpIdx[0][j] ;
                    double _tmpProb = lm.wordProb( _tmpIdx[1][i] , context ) ;
                    _tmpProb += _delta[j][t-1] ;
                    if ( _tmpProb > _curProb ){
                        _curProb = _tmpProb ;
                        _replaceIdx = j ;
                    }
                }

                _replaceM[i][t] = _map[_in1][_in2][_replaceIdx] ; 
                _backIdx[i][t] = _replaceIdx ; 
                _delta[i][t] = _curProb ;
            }

            for( int i = 0 ; i < _ansmapSize ; ++i ){
                _tmpIdx[0][i] = _tmpIdx[1][i] ;
            }
            _in1 = _ans1 ;
            _in2 = _ans2 ;
            _mapSize = _ansmapSize ;

        }


        for( int i = 0 ; i < _mapSize ; ++i ){
            context[0] = _tmpIdx[0][i] ;
            _delta[i][t] += lm.wordProb( _endIdx , context ) ;
            if( _delta[i][t] > _allProb ){
                _allProb = _delta[i][t] ;
                _allIdx = i ;
            }
        }

        string _ans[t+2] ;
        // init ans[]
        short _ansIdx ;
        _ans[t] = _replaceM[_allIdx][t] ;
        _ansIdx = _backIdx[_allIdx][t] ;
        // backtrace ans[]
        for( int i = t-1 ; i > 0 ; --i ){
            _ans[i] = _replaceM[_ansIdx][i] ;
            _ansIdx = _backIdx[_ansIdx][i] ;
        }
        _ans[t+1] = _map[_in1][_in2][_allIdx] ;

        // cerr << "t :" << t << endl ;
        cout << "<s> " ;
        // cerr << "output" ;
        for(int i = 0 ; i <= t ; ++i ){
            cout << _ans[i+1] << " " ;
            // cerr << " " << one_ans[i+1] ;
        }
        // cerr << endl ;
        cout << "</s>" << endl ;    
    }  
    return 0 ;
}
