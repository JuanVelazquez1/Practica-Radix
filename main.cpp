#include <iostream>
#include <vector>
#include <algorithm>

#include <tbb/tbb.h>
#include "oneapi/tbb/blocked_range.h"
#include "oneapi/tbb/parallel_for.h"

using namespace std;
using namespace oneapi;

/***
 * Given a n-size input vector, a n-size output vector and a mask.
 * Write in the output vector 1 in the indexes where the number in
 * the same position in the input vector has a bit set to 1 in the 
 * position given by the mask, write 0 otherwise. 
 */
void doMAP(int mask,vector<int> &x, int n, vector<int> &out)
{
    tbb::parallel_for(
        tbb::blocked_range<int>(0, n),
        
        // lambda function
        [&](tbb::blocked_range<int> r) {
            for (auto i = r.begin(); i != r.end(); i++) {
                if((x[i]&mask) >= 1){
                    out[i] = 1;
                }else{
                    out[i] = 0;
                }
            }
        }
    );
}

/***
 * Parallel scan with the previous 1's and 0's vector as input. 
 */
void doSCAN(vector<int> &out, const vector<int> &in, int n){
    tbb::parallel_scan(
        tbb::blocked_range<int>(0, n), //range
        0, //id
        // lambda function
        [&](tbb::blocked_range<int> r, int sum, bool is_final_scan){        
            int tmp = sum;
            for (int i = r.begin(); i < r.end(); ++i) {
                tmp = tmp + in[i];
                if (is_final_scan)
                    out[i] = tmp;
            }
            return tmp;
        },
        [&]( int left, int right ) {
            return left + right;
        }
    );
}

/***
 * Given the 1's and 0's vector, the scanned vector, the initial vector and an output vector:
 * If the value of the 1's and 0's vector is a 1, save the value
 * of the initial vector with the same index in the output vector 
 * in the position indicated by its scanned vector.
 * 
 * If the value of the 1's and 0's vector is a 0, save the value
 * of the initial vector with the same index in the output vector
 * in the position calculated by the next equation:
 * 
 *    output vector index = scanned vector max value + index in the initial vector - scanned vector index
 */
void doMAPFilter(vector<int> &bolMatch, vector<int> &ixMatch, vector<int> &x, vector<int> &out, int n){
    tbb::parallel_for(
        tbb::blocked_range<int>(0, n),
        // lambda function
        [&](tbb::blocked_range<int> r) {
            for (auto i = r.begin(); i < r.end(); i++) {
                if (bolMatch[i]){
                    out[ixMatch[i]-1] = x[i];    
                }else{ 
                    int last_pos = ixMatch[ixMatch.size() - 1];
                    int count = i - ixMatch[i];
                    out[last_pos + count] = x[i];    
                }
            }
        }
    );
}

int main(){

    //                     1     2     3     2     0     7     
    //static vector<int> x{0b001,0b010,0b011,0b010,0b000,0b111};
    static vector<int> x{12, 123, 35, 76, 9, 92, 103, 127, 128};

    // Indicate the maximum value of the array
    const int MAX_VALUE = 128;

    cout << endl;
    cout << "Initial vector: "<< endl;
    cout << "  ";
    for (int i: x){
        cout << i << ',';
    }
    cout << endl;

    vector<int> vect = x;
    int mask = 0b001;
	tbb::tick_count t0 = tbb::tick_count::now();

    // Calculate the number of iterations needed to sort the vector,
    // based on its highest value
 	for(int j = 0; j < int(log2(MAX_VALUE)) + 1; j++){
        //MAP operation
        vector<int> bolMatch(vect.size());
        doMAP(mask, vect, vect.size(), bolMatch);
 	
        /*cout << endl;
        cout << "Map vector " << j << ": "<< endl;
        cout << "  ";
        for (int i: bolMatch){
            cout << i << ',';
        }
        cout << endl;*/

        //SCAN
        vector<int> ixMatch(vect.size());
        doSCAN(ixMatch, bolMatch,  vect.size()); //get index order
    

        /*cout << "Scan vector " << j << ": " << endl;
        cout << "  ";
        for (int i: ixMatch){
            cout << i << ',';
        }
        cout << endl;*/

        //JOIN
        vector<int> filtered_results(vect.size());
        doMAPFilter(bolMatch,ixMatch,vect,filtered_results, vect.size());
    
        /*cout << "Filtered vector " << j << ": " << endl;
        cout << "  ";
        for (int i: filtered_results){
            cout << i << ',';
        }
        cout << endl;*/

        // Update the vector for the next iteration
        vect = filtered_results;
        // Update the mask for the next iteration
        mask = mask << 1;
    }

    cout << endl;
    cout << "RESULT OF RADIX: " << endl;
    cout << "  ";
    for (int i: vect){
        cout << i << ',';
    }
    cout << endl;

 	cout << "\nTime: " << (tbb::tick_count::now()-t0).seconds() << "seconds" << endl;
 	
    return 0;
}
