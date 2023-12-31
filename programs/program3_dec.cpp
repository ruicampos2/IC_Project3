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
    // Function that converts array of ints (1 or 0) to int
    auto bits_to_int = [](vector<int> bits) {
        return accumulate(bits.begin(), bits.end(), 0, [](int acc, int bit) {
            return (acc << 1) | bit;
        });
    };

    // Function that converts array of bits to char
    auto bits_to_char = [](vector<int> bits) {
        return accumulate(bits.begin(), bits.end(), 0, [](int acc, int bit) {
            return (acc << 1) | bit;
        });
    };

    // Function to predict value based on given integers a, b, and c
    auto predict = [](int a, int b, int c) {
        if (c >= max(a, b))
            return min(a, b);
        else if (c <= min(a, b))
            return max(a, b);
        else
            return a + b - c;
    };

    if (argc != 3) {
        cout << "Usage: " << argv[0] << " <input file> <output file>" << endl;
        return 1;
    }
    string input_file = argv[1];
    BitStream bs(input_file, "r");
    string output_file = argv[2];
    
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

    vector<int> v_searchD = bs.readBits(16);
    vector<int> v_keyFrame = bs.readBits(16);
    vector<int> v_paddedWidth = bs.readBits(16);
    vector<int> v_paddedHeight = bs.readBits(16);

    vector<int> v_Ybits_size = bs.readBits(32);
    vector<int> v_Cbbits_size = bs.readBits(32);
    vector<int> v_Crbits_size = bs.readBits(32);
    vector<int> v_Ym_size = bs.readBits(32);
    vector<int> v_Cbm_size = bs.readBits(32);
    vector<int> v_Crm_size = bs.readBits(32);
    vector<int> v_motionXSize = bs.readBits(32);
    vector<int> v_motionYSize = bs.readBits(32);

    int width = bits_to_int(v_width);
    int height = bits_to_int(v_height);

    int num_frames = bits_to_int(v_num_frames);
    int color_space = bits_to_int(v_color_space);

    int aspect_ratio_1 = bits_to_int(v_aspect_ratio_1);
    int aspect_ratio_2 = bits_to_int(v_aspect_ratio_2);
    int frame_rate_1 = bits_to_int(v_frame_rate_1);
    int frame_rate_2 = bits_to_int(v_frame_rate_2);

    char interlace = bits_to_char(v_interlace);
    int blockSize = bits_to_int(v_bs);

    int keyFramePeriod = bits_to_int(v_keyFrame);
    int padded_width = bits_to_int(v_paddedWidth);
    int padded_height = bits_to_int(v_paddedHeight);

    int Ybits_size = bits_to_int(v_Ybits_size);
    int Cbbits_size = bits_to_int(v_Cbbits_size);
    int Crbits_size = bits_to_int(v_Crbits_size);
    int Ym_size = bits_to_int(v_Ym_size);
    int Cbm_size = bits_to_int(v_Cbm_size);
    int Crm_size = bits_to_int(v_Crm_size);
    int motionXSize = bits_to_int(v_motionXSize);
    int motionYSize = bits_to_int(v_motionYSize);

    //The output file is a YUV4MPEG2 file
    //write the header
    ofstream out(output_file, ios::out | ios::binary);

    //READING MOTION VECTORS
    vector<int> motionX = bs.readBits(motionXSize);
    vector<int> motionY = bs.readBits(motionYSize);

    string motionX_str = accumulate(motionX.begin(), motionX.end(), string(), [](string& acc, int bit) {
        return acc + to_string(bit);
    });

    string motionY_str = accumulate(motionY.begin(), motionY.end(), string(), [](string& acc, int bit) {
        return acc + to_string(bit);
    });

    Golomb gMotion;
    vector<int> motionVectorXs = gMotion.decode(motionX_str, 8);
    vector<int> motionVectorYs = gMotion.decode(motionY_str, 8);

    //READING M VALUES
    vector<int> Ym, Cbm, Crm;

    for (int i = 0; i < Ym_size; i++) {
        vector<int> v_Ym = bs.readBits(8);
        int Ym_i = accumulate(v_Ym.rbegin(), v_Ym.rend(), 0, [](int acc, int bit) {
            return (acc << 1) | bit;
        });
        Ym.push_back(Ym_i);
    }

    for (int i = 0; i < Cbm_size; i++) {
        vector<int> v_Cbm = bs.readBits(8);
        int Cbm_i = accumulate(v_Cbm.rbegin(), v_Cbm.rend(), 0, [](int acc, int bit) {
            return (acc << 1) | bit;
        });
        Cbm.push_back(Cbm_i);
    }

    for (int i = 0; i < Crm_size; i++) {
        vector<int> v_Crm = bs.readBits(8);
        int Crm_i = accumulate(v_Crm.rbegin(), v_Crm.rend(), 0, [](int acc, int bit) {
            return (acc << 1) | bit;
        });
        Crm.push_back(Crm_i);
    }

    //READING YUV VALUES
    vector<int> Ybits = bs.readBits(Ybits_size);
    vector<int> Cbbits = bs.readBits(Cbbits_size);
    vector<int> Crbits = bs.readBits(Crbits_size);

    string Yencodedstring = accumulate(Ybits.begin(), Ybits.end(), string(), [](string& acc, int bit) {
        return acc + to_string(bit);
    });

    string Cbencodedstring = accumulate(Cbbits.begin(), Cbbits.end(), string(), [](string& acc, int bit) {
        return acc + to_string(bit);
    });

    string Crencodedstring = accumulate(Crbits.begin(), Crbits.end(), string(), [](string& acc, int bit) {
        return acc + to_string(bit);
    });


    //DECODE YUV VALUES
    Golomb g;
    vector<int> Ydecoded = g.decodeMultiple(Yencodedstring, Ym, blockSize);
    vector<int> Cbdecoded = g.decodeMultiple(Cbencodedstring, Cbm, blockSize);
    vector<int> Crdecoded = g.decodeMultiple(Crencodedstring, Crm, blockSize);

    Mat YMat = Mat(height, width, CV_8UC1);
    Mat UMat;
    Mat VMat;

    if (color_space == 420) {
        UMat = Mat(height/2, width/2, CV_8UC1);
        VMat = Mat(height/2, width/2, CV_8UC1);
    } else if (color_space == 422) {
        UMat = Mat(height, width/2, CV_8UC1);
        VMat = Mat(height, width/2, CV_8UC1);
    } else if (color_space == 444) {
        UMat = Mat(height, width, CV_8UC1);
        VMat = Mat(height, width, CV_8UC1);
    }

    //undo the predictions
    int pixel_idx = 0;
    int pixel_idx2 = 0;

    Mat keyYmat, keyUmat, keyVmat;
    int motionX_idx = 0;
    int motionY_idx = 0;
    //FOR EACH FRAME
    for (int n = 0; n < num_frames; n++) {

        //PREDICTION 
        //INTRA-FRAME PREDICTION (keyFrame)
        //if its the first frame, or if the current frame is a keyframe, do not use inter frame prediction
        if(n == 0 || (n % keyFramePeriod == 0)){
            UMat = Mat(height/2, width/2, CV_8UC1);
            VMat = Mat(height/2, width/2, CV_8UC1);

            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    if (i == 0 && j == 0) {
                        YMat.at<uchar>(i,j) = Ydecoded[pixel_idx];
                        UMat.at<uchar>(i,j) = Cbdecoded[pixel_idx2];
                        VMat.at<uchar>(i,j) = Crdecoded[pixel_idx2];
                        pixel_idx2++;
                    } else if (i == 0) {
                        YMat.at<uchar>(i, j) = Ydecoded[pixel_idx] + YMat.at<uchar>(i, j-1);
                        if (j < (width/2)) {
                            UMat.at<uchar>(i,j) = Cbdecoded[pixel_idx2] + UMat.at<uchar>(i, j-1);
                            VMat.at<uchar>(i,j) = Crdecoded[pixel_idx2] + VMat.at<uchar>(i, j-1);
                            pixel_idx2++;
                        }
                    } else if (j == 0) {
                        YMat.at<uchar>(i, j) = Ydecoded[pixel_idx] + YMat.at<uchar>(i-1, j);
                        if (i < (height/2)) {
                            UMat.at<uchar>(i,j) = Cbdecoded[pixel_idx2] + UMat.at<uchar>(i-1, j);
                            VMat.at<uchar>(i,j) = Crdecoded[pixel_idx2] + VMat.at<uchar>(i-1, j);
                            pixel_idx2++;
                        }
                    } else {
                        YMat.at<uchar>(i, j) = Ydecoded[pixel_idx] + predict(YMat.at<uchar>(i, j-1), YMat.at<uchar>(i-1, j), YMat.at<uchar>(i-1, j-1));
                        if(i < (height/2) && j < (width/2)){
                            UMat.at<uchar>(i,j) = Cbdecoded[pixel_idx2] + predict(UMat.at<uchar>(i, j-1), UMat.at<uchar>(i-1, j), UMat.at<uchar>(i-1, j-1));
                            VMat.at<uchar>(i,j) = Crdecoded[pixel_idx2] + predict(VMat.at<uchar>(i, j-1), VMat.at<uchar>(i-1, j), VMat.at<uchar>(i-1, j-1));
                            pixel_idx2++;
                        }
                    }
                    pixel_idx++;
                }
            }
            //KEYFRAME SAVING
            keyYmat = YMat.clone();
            keyUmat = UMat.clone();
            keyVmat = VMat.clone();
            
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
                            Cb_vector.push_back(UMat.at<uchar>(i, j));
                            Cr_vector.push_back(VMat.at<uchar>(i, j));
                        }
                    }
                } else if(color_space == 422){
                    if (i < width/2) {
                        for(int j = 0; j < width/2; j++){
                            Cb_vector.push_back(UMat.at<uchar>(i, j));
                            Cr_vector.push_back(VMat.at<uchar>(i, j));
                        }
                    }
                } else if(color_space == 444){
                    for(int j = 0; j < width; j++){
                        Cb_vector.push_back(UMat.at<uchar>(i, j));
                        Cr_vector.push_back(VMat.at<uchar>(i, j));
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
        } else {
            //INTER-FRAME PREDICTION (non-keyFrame, block by block)
            //motion compensation (block by block) with the keyframe
            //go block by block through the Y, U, and V Mat object to make predictions
            YMat = Mat(padded_height, padded_width, CV_8UC1);
            UMat = Mat(padded_height/2, padded_width/2, CV_8UC1);
            VMat = Mat(padded_height/2, padded_width/2, CV_8UC1);
            
            //FRAME CONSTRUCTION
            Mat keyFrameMat = Mat(padded_height, padded_width, CV_8UC3);
            //colorspace 420
            for (int i = 0; i < padded_height; i++){
                for (int j = 0; j < padded_width; j++){
                    int half_i = i/2;
                    int half_j = j/2;
                    keyFrameMat.at<Vec3b>(i, j)[0] = keyYmat.at<uchar>(i, j);
                    keyFrameMat.at<Vec3b>(i, j)[1] = keyUmat.at<uchar>(half_i, half_j);
                    keyFrameMat.at<Vec3b>(i, j)[2] = keyVmat.at<uchar>(half_i, half_j);
                }
            }

            int num_blocks_width = padded_width/blockSize;    
            int num_blocks_height = padded_height/blockSize;   
            for (int bw = 0; bw < num_blocks_width; bw++){
                for(int bh = 0; bh < num_blocks_height; bh++){
                    for (int i = 0; i < blockSize; i++){
                        for (int j = 0; j < blockSize; j++){
                            //Getting the current block (predicted)
                            YMat.at<uchar>(bh*blockSize + i, bw*blockSize + j) = keyFrameMat.at<Vec3b>(bh*blockSize + i + motionVectorYs[motionY_idx], bw*blockSize + j + motionVectorXs[motionX_idx])[0] + Ydecoded[pixel_idx];
                            if(n==7){
                                if ((bh*blockSize + i + motionVectorYs[motionY_idx]) > padded_height || (bh*blockSize + i + motionVectorYs[motionY_idx]) < 0){
                                    return -1;
                                }
                                if ((bw*blockSize + j + motionVectorXs[motionX_idx]) > padded_width || (bw*blockSize + j + motionVectorXs[motionX_idx]) < 0){
                                    return -1;
                                }
                            }
                            pixel_idx++;
                            if (i % 2 == 0 and j % 2 == 0){
                                UMat.at<uchar>((bh*blockSize + i)/2, (bw*blockSize + j)/2) = keyFrameMat.at<Vec3b>(bh*blockSize + i + motionVectorYs[motionY_idx], bw*blockSize + j + motionVectorXs[motionX_idx])[1] + Cbdecoded[pixel_idx2];
                                VMat.at<uchar>((bh*blockSize + i)/2, (bw*blockSize + j)/2) = keyFrameMat.at<Vec3b>(bh*blockSize + i + motionVectorYs[motionY_idx], bw*blockSize + j + motionVectorXs[motionX_idx])[2] + Crdecoded[pixel_idx2];
                                pixel_idx2++;
                            }

                        }
                    }
                    motionY_idx++;
                    motionX_idx++;
                }
            }
            vector<int> Y_vector;
            vector<int> Cb_vector;
            vector<int> Cr_vector;

            //convert the matrix back to a vector
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    Y_vector.push_back(YMat.at<uchar>(i, j));
                }

                if (color_space == 420) {
                    if (i < height / 2 && i < width / 2) {
                        for (int j = 0; j < width / 2; j++) {
                            Cb_vector.push_back(UMat.at<uchar>(i, j));
                            Cr_vector.push_back(VMat.at<uchar>(i, j));
                        }
                    }
                } else if (color_space == 422) {
                    if (i < width / 2) {
                        for (int j = 0; j < width / 2; j++) {
                            Cb_vector.push_back(UMat.at<uchar>(i, j));
                            Cr_vector.push_back(VMat.at<uchar>(i, j));
                        }
                    }
                } else if (color_space == 444) {
                    for (int j = 0; j < width; j++) {
                        Cb_vector.push_back(UMat.at<uchar>(i, j));
                        Cr_vector.push_back(VMat.at<uchar>(i, j));
                    }
                }
            }
            for (long unsigned int i = 0; i < Y_vector.size(); i++) {
                char byte = static_cast<char>(Y_vector[i]);
                out.write(&byte, sizeof(byte));
            }

            for (long unsigned int i = 0; i < Cb_vector.size(); i++) {
                char byte = static_cast<char>(Cb_vector[i]);
                out.write(&byte, sizeof(byte));
            }

            for (long unsigned int i = 0; i < Cr_vector.size(); i++) {
                char byte = static_cast<char>(Cr_vector[i]);
                out.write(&byte, sizeof(byte));
            }
        }

    }

    //close the file
    out.close();
    cout << "File saved in opencv-bin folder " << endl;
    return 0;
}