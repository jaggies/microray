#include "math.h"

float atof(const char* ptr) {
    float result = 0.0f;
	float fraction = 0.1f;
    int isNegative = 0;
    int isNegativeExponent = 0;
	int dotSeen = 0;
	int exponentSeen = 0;
	int exponent = 0;
    if (ptr && *ptr != 0) do {
        int ch = *ptr;
        if (ch == '-' || ch == '+') {
			if (!exponentSeen) {
            	isNegative = ch == '-';
			} else {
				isNegativeExponent = ch == '-';
			}
        } else if (ch == '.') {
            dotSeen = 1;
        } else if (ch >= '0' && ch <= '9') {
			if (!exponentSeen) {
				if (!dotSeen) {
					result = result * 10.0f + (float) (ch - '0');
				} else {
					result = result + (float) (ch - '0') * fraction;
					fraction *= 0.1f;
				}
			} else {
				exponent = exponent * 10 + (ch - '0');
			}
		} else if (ch == 'e' || ch == 'E') {
			exponentSeen = 1;	
		}
    } while (*++ptr != 0);

	if (isNegative) {
		result = - result;
	}

	if (exponent) {
		while (exponent--) {
			result *= isNegativeExponent ? 0.1f : 10.0f;
		}
	}
		
    return result;
}
