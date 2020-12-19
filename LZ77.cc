#include <iostream>
#include <string>
#include "LZ77.h"

using namespace std;

void LZ77(char S[13], int len, int window, char EOS, LDPair * output) {
#pragma HLS INLINE
uint8_t o = 0, l = 0, o2 = 0, l2 = 0;
char c = 'Z';
int i, j, k;
LDPair * m = output;

for (i = 0; i < len; i++) {
    l = l2 = o2 = 0;
    k = i;
    c = S[i];
    for (j = i-window; j < len; j++) {
        if (j >= i && l == 0) {
            break; // we are past search-buffer
        }
        if (j >= 0 && S[j] == S[k]) {
            o = k-j;
            l++;
            k++;
        }
        else {
            if (l > 0) {
                j -= l; // Reset search to character right past previous match
            }
            l = 0;
            k = i;
        }
        if (l >= l2) {
            l2 = l;
            o2 = o;
            c = S[k];
        }
        if (k >= len) {
            c = S[len-1];
            //c = '$'; // string end symbol
            o2 = 0;
            l2 = 0;
            break;
        }
    }
    i += l2;
    cout << "(" << unsigned(o2) << ", " << unsigned(l2) << ", " << c << ") ";
    m->Set(o2, l2, c);
    cout << unsigned(m->offset) << ", " << unsigned(m->length) << ", " << m->symbol << endl;
    m->next = new LDPair;
    m = m->next;
    m->next = NULL;
    m->symbol = EOS;
}

return;
}
