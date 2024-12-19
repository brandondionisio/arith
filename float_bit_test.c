#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

uint64_t encode_9_bit_float(float a_val)
{
        a_val *= 511;

        if (a_val < 0) {
                a_val = 0;
        } else if (a_val > 511) {
                a_val = 511;
        }
        return (uint64_t)a_val;
}

int64_t encode_5_bit_float(float bcd_val)
{
        /* range of -16 < bcd_val < 16 */
        bcd_val *= (15 / 0.3);

        if (bcd_val < -15) {
                bcd_val = -15;
        } else if (bcd_val > 15) {
                bcd_val = 15;
        }

        return bcd_val;
}

float decode_9_bit_float(uint64_t u_a)
{
        return (u_a * 1.0) / 511.0;
}

float decode_5_bit_float(int64_t u_bcd)
{
        //return (u_bcd * 1.0) / 16.0 * 0.3;
        return u_bcd * (0.3 / 15);
}

int main()
{
        float test1 = .50295;
        // 0.00195 intervals
        uint64_t t1 = encode_9_bit_float(test1);
        printf("test 1 float is: %.04f\n", decode_9_bit_float(t1));

        // float test2 = -.04;
        // int64_t t2 = encode_5_bit_float(test2);
        // printf("test 2 int is %ld\n", t2);
        // printf("test 2 float is: %.03f\n", decode_5_bit_float(t2));
}