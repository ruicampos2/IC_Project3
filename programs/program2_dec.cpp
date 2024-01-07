#include "bitStream.h"
#include "opencv2/opencv.hpp"
#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include "Reader.h"
#include "golomb.h"

using namespace std;
using namespace cv;

int main(int argc, char* argv[]){
    //function that converts array of ints (1 or 0) to int
    auto bits_to_int = [](vector<int> bits) {
        int c = 0;
        for(long unsigned int i = 0; i < bits.size(); i++) c += bits[i] * pow(2, bits.size() - i - 1);
        return c;
    };

    //function that converts array of bits to char
    auto bits_to_char = [](vector<int> bits) {
        int c = 0;
        for(long unsigned int i = 0; i < bits.size(); i++) c += bits[i] * pow(2, bits.size() - i - 1);
        return c;
    };

    auto predict = [](int a, int b, int c) {
        if (c >= max(a, b))
            return min(a, b);   //min(left, above) if  left top >= max(left, above)
        else if (c <= min(a, b))
            return max(a, b);   //max(left, above) if  left top <= min(left, above)
        else
            return a + b - c;   //otherwise, left + above - left top
    };


    if(argc != 3){
        cout << "Usage: " << argv[0] << " <input file> <output file>" << endl;
        return 1;
    }
    string input_file = argv[1];
    BitStream bs(input_file, "r");
    string output_file = argv[2];

    //The output file is a YUV4MPEG2 file
    //write the header
    ofstream out(output_file, ios::out | ios::binary);


    vector<int> v_width = bs.readBits(16);
    vector<int> v_height = bs.readBits(16);
    vector<int> v_num_frames = bs.readBits(16);
    vector<int> v_color_space = bs.readBits(16);
    vector<int> v_aspect_ratio_1 = bs.readBits(16);
    vector<int> v_aspect_ratio_2 = bs.readBits(16);
    vector<int> v_frame_rate_1 = bs.readBits(16);
    vector<int> v_frame_rate_2 = bs.readBits(16);
    vector<int> v_interlace = bs.readBits(8);
    vector<int> v_bs = bs.readBits(16);
    vector<int> v_Ybits_size = bs.readBits(32);
    vector<int> v_Cbbits_size = bs.readBits(32);
    vector<int> v_Crbits_size = bs.readBits(32);
    vector<int> v_Ym_size = bs.readBits(32);
    vector<int> v_Cbm_size = bs.readBits(32);
    vector<int> v_Crm_size = bs.readBits(32);

    int width = bits_to_int(v_width);
    int height = bits_to_int(v_height);
    int num_frames = bits_to_int(v_num_frames);
    int color_space = bits_to_int(v_color_space);
    int aspect_ratio_1 = bits_to_int(v_aspect_ratio_1);
    int aspect_ratio_2 = bits_to_int(v_aspect_ratio_2);
    int frame_rate_1 = bits_to_int(v_frame_rate_1);
    int frame_rate_2 = bits_to_int(v_frame_rate_2);
    char interlace = bits_to_char(v_interlace);
    int bs_size = bits_to_int(v_bs);
    int Ybits_size = bits_to_int(v_Ybits_size);
    int Cbbits_size = bits_to_int(v_Cbbits_size);
    int Crbits_size = bits_to_int(v_Crbits_size);
    int Ym_size = bits_to_int(v_Ym_size);
    int Cbm_size = bits_to_int(v_Cbm_size);
    int Crm_size = bits_to_int(v_Crm_size);

    //write the header
    if(color_space != 420)
        out << "YUV4MPEG2 W" << width << " H" << height << " F" << frame_rate_1 << ":" << frame_rate_2 << " I" << interlace << " A" << aspect_ratio_1 << ":" << aspect_ratio_2 << " C" << color_space << endl;
    else 
        out << "YUV4MPEG2 W" << width << " H" << height << " F" << frame_rate_1 << ":" << frame_rate_2 << " I" << interlace << " A" << aspect_ratio_1 << ":" << aspect_ratio_2 << endl;

    //write to the file FRAME
    out << "FRAME" << endl;

    //READING M VALUES
    vector<int> Ym;
    for(int i = 0; i < Ym_size; i++){
        vector<int> v_Ym = bs.readBits(8);
        int Ym_i = 0;
        for (long unsigned int j = 0; j < v_Ym.size(); j++) Ym_i += v_Ym[j] * pow(2, v_Ym.size() - j - 1);
        Ym.push_back(Ym_i);
    }

    vector<int> Cbm;
    for(int i = 0; i < Cbm_size; i++){
        vector<int> v_Cbm = bs.readBits(8);
        int Cbm_i = 0;
        for (long unsigned int j = 0; j < v_Cbm.size(); j++) Cbm_i += v_Cbm[j] * pow(2, v_Cbm.size() - j - 1);
        Cbm.push_back(Cbm_i);
    }

    vector<int> Crm;
    for(int i = 0; i < Crm_size; i++){
        vector<int> v_Crm = bs.readBits(8);
        int Crm_i = 0;
        for (long unsigned int j = 0; j < v_Crm.size(); j++) Crm_i += v_Crm[j] * pow(2, v_Crm.size() - j - 1);
        Crm.push_back(Crm_i);
    }


    //READING YUV VALUES
    vector<int> Ybits = bs.readBits(Ybits_size);
    vector<int> Cbbits = bs.readBits(Cbbits_size);
    vector<int> Crbits = bs.readBits(Crbits_size);

    cout << Cbbits_size << endl;
    cout << Crbits_size << endl;

    string Yencodedstring = "";
    for(long unsigned int i = 0; i < Ybits.size(); i++) Yencodedstring += Ybits[i] + '0';
    string Cbencodedstring = "";
    string Crencodedstring = "";
    for(long unsigned int i = 0; i < Cbbits.size(); i++) {
        Cbencodedstring += Cbbits[i] + '0';
    }
    for(long unsigned int i = 0; i < Crbits.size(); i++) {
        Crencodedstring += Crbits[i] + '0';
    }



    //DECODE YUV VALUES
    Golomb g;
    vector<int> Ydecoded = g.decodeMultiple(Yencodedstring, Ym, bs_size);
    vector<int> Cbdecoded = g.decodeMultiple(Cbencodedstring, Cbm, bs_size);
    vector<int> Crdecoded = g.decodeMultiple(Crencodedstring, Crm, bs_size);

    Mat YMat = Mat(height, width, CV_8UC1);
    Mat u;
    Mat v;

    if (color_space == 420) {
        u = Mat(height/2, width/2, CV_8UC1);
        v = Mat(height/2, width/2, CV_8UC1);
    } else if (color_space == 422) {
        u = Mat(height, width/2, CV_8UC1);
        v = Mat(height, width/2, CV_8UC1);
    } else if (color_space == 444) {
        u = Mat(height, width, CV_8UC1);
        v = Mat(height, width, CV_8UC1);
    }



    //undo the predictions
    int pixel_idx = 0;
    int pixel_idx2 = 0;
    for (int n = 0; n < num_frames; n++) {
        YMat = Mat(height, width, CV_8UC1);
        if (color_space == 420) {
            u = Mat(height/2, width/2, CV_8UC1);
            v = Mat(height/2, width/2, CV_8UC1);
        } else if (color_space == 422) {
            u = Mat(height, width/2, CV_8UC1);
            v = Mat(height, width/2, CV_8UC1);
        } else if (color_space == 444) {
            u = Mat(height, width, CV_8UC1);
            v = Mat(height, width, CV_8UC1);
        }

        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (i == 0 && j == 0) {
                    YMat.at<uchar>(i, j) = Ydecoded[pixel_idx];
                    u.at<uchar>(i,j) = Cbdecoded[pixel_idx2];
                    v.at<uchar>(i,j) = Crdecoded[pixel_idx2];
                    pixel_idx++;
                    pixel_idx2++;
                } else if (i == 0) {
                    YMat.at<uchar>(i, j) = Ydecoded[pixel_idx] + YMat.at<uchar>(i, j-1);
                    if (color_space == 420 || color_space == 422) {
                        if (j < (width/2)) {
                            u.at<uchar>(i,j) = Cbdecoded[pixel_idx2] + u.at<uchar>(i, j-1);
                            v.at<uchar>(i,j) = Crdecoded[pixel_idx2] + v.at<uchar>(i, j-1);
                            pixel_idx2++;
                        }
                    } else if (color_space == 444) {
                        u.at<uchar>(i,j) = Cbdecoded[pixel_idx2] + u.at<uchar>(i, j-1);
                        v.at<uchar>(i,j) = Crdecoded[pixel_idx2] + v.at<uchar>(i, j-1);
                        pixel_idx2++;
                    }
                    pixel_idx++;
                } else if (j == 0) {
                    YMat.at<uchar>(i, j) = Ydecoded[pixel_idx] + YMat.at<uchar>(i-1, j);
                    if (color_space == 420) {
                        if (i < (height/2)) {
                            u.at<uchar>(i,j) = Cbdecoded[pixel_idx2] + u.at<uchar>(i-1, j);
                            v.at<uchar>(i,j) = Crdecoded[pixel_idx2] + v.at<uchar>(i-1, j);
                            pixel_idx2++;
                        }
                    } else if(color_space == 422 || color_space == 444){
                        u.at<uchar>(i,j) = Cbdecoded[pixel_idx2] + u.at<uchar>(i-1, j);
                        v.at<uchar>(i,j) = Crdecoded[pixel_idx2] + v.at<uchar>(i-1, j);
                        pixel_idx2++;
                    }
                    pixel_idx++;
                } else {
                    YMat.at<uchar>(i, j) = Ydecoded[pixel_idx] + predict(YMat.at<uchar>(i, j-1), YMat.at<uchar>(i-1, j), YMat.at<uchar>(i-1, j-1));
                    if(color_space == 420){
                        if(i < (height/2) && j < (width/2)){
                            u.at<uchar>(i,j) = Cbdecoded[pixel_idx2] + predict(u.at<uchar>(i, j-1), u.at<uchar>(i-1, j), u.at<uchar>(i-1, j-1));
                            v.at<uchar>(i,j) = Crdecoded[pixel_idx2] + predict(v.at<uchar>(i, j-1), v.at<uchar>(i-1, j), v.at<uchar>(i-1, j-1));
                            pixel_idx2++;
                        }
                    } else if(color_space == 422){
                        if(j < (width/2)){
                            u.at<uchar>(i,j) = Cbdecoded[pixel_idx2] + predict(u.at<uchar>(i, j-1), u.at<uchar>(i-1, j), u.at<uchar>(i-1, j-1));
                            v.at<uchar>(i,j) = Crdecoded[pixel_idx2] + predict(v.at<uchar>(i, j-1), v.at<uchar>(i-1, j), v.at<uchar>(i-1, j-1));
                            pixel_idx2++;
                        }
                    } else if(color_space == 444){
                        u.at<uchar>(i,j) = Cbdecoded[pixel_idx2] + predict(u.at<uchar>(i, j-1), u.at<uchar>(i-1, j), u.at<uchar>(i-1, j-1));
                        v.at<uchar>(i,j) = Crdecoded[pixel_idx2] + predict(v.at<uchar>(i, j-1), v.at<uchar>(i-1, j), v.at<uchar>(i-1, j-1));
                        pixel_idx2++;
                    }
                    pixel_idx++;
                }
            }
        }

        //convert the matrix back to a vector
        vector<int> Y_vector;
        vector<int> Cb_vector;
        vector<int> Cr_vector;
        for(int i = 0; i < height; i++){
            for(int j = 0; j < width; j++){
                Y_vector.push_back(YMat.at<uchar>(i, j));
            }
            if(color_space == 420){
                if (i < height/2 && i < width/2) {
                    for(int j = 0; j < width/2; j++){
                        Cb_vector.push_back(u.at<uchar>(i, j));
                        Cr_vector.push_back(v.at<uchar>(i, j));
                    }
                }
            } else if(color_space == 422){
                if (i < width/2) {
                    for(int j = 0; j < width/2; j++){
                        Cb_vector.push_back(u.at<uchar>(i, j));
                        Cr_vector.push_back(v.at<uchar>(i, j));
                    }
                }
            } else if(color_space == 444){
                for(int j = 0; j < width; j++){
                    Cb_vector.push_back(u.at<uchar>(i, j));
                    Cr_vector.push_back(v.at<uchar>(i, j));
                }
            }
        }

        //write the Y_vector to the file
        for(long unsigned int i = 0; i < Y_vector.size(); i++){
            //convert the int to a byte
            char byte = (char)Y_vector[i];
            //write the byte to the file
            out.write(&byte, sizeof(byte));
        }

        //write the Cb_vector to the file
        for(long unsigned int i = 0; i < Cb_vector.size(); i++){
            //convert the int to a byte
            char byte = (char)Cb_vector[i];
            //write the byte to the file
            out.write(&byte, sizeof(byte));
        }

        //write the Cr_vector to the file
        for(long unsigned int i = 0; i < Cr_vector.size(); i++){
            //convert the int to a byte
            char byte = (char)Cr_vector[i];
            //write the byte to the file
            out.write(&byte, sizeof(byte));
        }

        if(n < num_frames - 1) out << "FRAME" << endl;
    }


    //close the file
    out.close();
    cout << "File saved in opencv-bin folder << endl;
    return 0;
}
