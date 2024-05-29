#ifndef _jnd_
#define _jnd_

#pragma comment(lib, "jnd_mat.lib")
#include "jnd_mat.h"

#include <stdlib.h>

#define max_W 4096
#define max_H 2048
#define mi_size 4 // compute jnd in mi_size x mi_size
#define W_mi max_W / mi_size
#define H_mi max_H / mi_size

bool init_flag = false;  // flag of init_buffer()

double jnd_factor[max_H][max_W] = { 0 };
extern double rdo_factor[H_mi][W_mi] = { 0 };

extern double sum_last = 0;  // Y sum of last frame
extern UINT8 *img_last = 0;  // buffer of last frame

extern void init_buffer(AV1_COMP *cpi) {
  if (init_flag == false) {
    int picWidthY = cpi->source->crop_widths[0];
    int picHeightY = cpi->source->crop_heights[0];
    img_last = (UINT8 *)malloc(sizeof(UINT8) * picHeightY * picWidthY);
    init_flag = true;
  }
}

extern void jnd_calculation_mi(AV1_COMP *cpi) {
  // Y buffer
  uint8_t *src = cpi->source->buffers[0];

  // picture parameter
  int picWidthY = cpi->source->crop_widths[0];
  int picHeightY = cpi->source->crop_heights[0];
  int picStrideY = cpi->source->strides[0];

  // frame & JND buffer
  UINT8 *img_input = (UINT8 *)malloc(sizeof(UINT8) * picHeightY * picWidthY);
  double *jnd_factor1 =
      (double *)malloc(sizeof(double) * picHeightY * picWidthY);
  double *jnd_factor2 =
      (double *)malloc(sizeof(double) * picHeightY * picWidthY);
  double *jnd_factor3 =
      (double *)malloc(sizeof(double) * picHeightY * picWidthY);
  double *jnd_factor4 =
      (double *)malloc(sizeof(double) * picHeightY * picWidthY);
  double *jnd_factor5 =
      (double *)malloc(sizeof(double) * picHeightY * picWidthY);

  double sum_cur = 0; // Y sum of current frame
  for (int i = 0; i < picHeightY; ++i) {
    for (int j = 0; j < picWidthY; ++j) {
      img_input[j + i * picWidthY] = src[j + i * picStrideY];
      sum_cur += src[j + i * picStrideY];
    }
  }

  // jnd function input and output
  mxArray *img_input_array =
      mxCreateNumericMatrix(picWidthY, picHeightY, mxUINT8_CLASS, mxREAL);
  mxArray *img_output_array1 =
      mxCreateDoubleMatrix(picWidthY, picHeightY, mxREAL);
  mxArray *img_output_array2 =
      mxCreateDoubleMatrix(picWidthY, picHeightY, mxREAL);
  mxArray *img_output_array3 =
      mxCreateDoubleMatrix(picWidthY, picHeightY, mxREAL);
  mxArray *img_output_array4 =
      mxCreateDoubleMatrix(picWidthY, picHeightY, mxREAL);
  mxArray *img_output_array5 =
      mxCreateDoubleMatrix(picWidthY, picHeightY, mxREAL);

  // get input
  memcpy((UINT8 *)mxGetPr(img_input_array), img_input,
         sizeof(UINT8) * picWidthY * picHeightY);

  // call func
  int nargout = 5;
  mlfFunc_JND_modeling_pattern_complexity(
      nargout, &img_output_array1, &img_output_array2, &img_output_array3,
      &img_output_array4, &img_output_array5, img_input_array);

  // get output
  memcpy(jnd_factor1, mxGetPr(img_output_array1),
         sizeof(double) * picWidthY * picHeightY);
  memcpy(jnd_factor2, mxGetPr(img_output_array2),
         sizeof(double) * picWidthY * picHeightY);
  memcpy(jnd_factor3, mxGetPr(img_output_array3),
         sizeof(double) * picWidthY * picHeightY);
  memcpy(jnd_factor4, mxGetPr(img_output_array4),
         sizeof(double) * picWidthY * picHeightY);
  memcpy(jnd_factor5, mxGetPr(img_output_array5),
         sizeof(double) * picWidthY * picHeightY);

  mxDestroyArray(img_input_array);
  mxDestroyArray(img_output_array1);
  mxDestroyArray(img_output_array2);
  mxDestroyArray(img_output_array3);
  mxDestroyArray(img_output_array4);
  mxDestroyArray(img_output_array5);

  // jnd factor weigh
  // _, jnd_map, jnd_LA, jnd_CM, jnd_PM, jnd_PM_p 
  double weight[6] = { 0, 1, 0, 10, 0, 0 };
  for (int i = 0; i < picHeightY; ++i) {
    for (int j = 0; j < picWidthY; ++j) {
      jnd_factor[i][j] = (weight[1] * jnd_factor1[j + i * picWidthY] +
                          weight[2] * jnd_factor2[j + i * picWidthY] +
                          weight[3] * jnd_factor3[j + i * picWidthY] +
                          weight[4] * jnd_factor4[j + i * picWidthY] +
                          weight[5] * jnd_factor5[j + i * picWidthY]) /
                         (weight[1] + weight[2] + weight[3] + weight[4] + weight[5]);
    }
  }

#if 1 // temporal JND
  // sum_last /= picHeightY * picWidthY;
  sum_cur /= picHeightY * picWidthY;
  double err = sum_cur - sum_last;
  for (int i = 0; i < picHeightY; ++i) {
    for (int j = 0; j < picWidthY; ++j) {
      double x = (src[j + i * picStrideY] -
                  img_last[j + i * picWidthY] + err) * 0.5;

      if (abs(x) <= 5) {
        x = 0.8;
      } else if (x > 5) {
        x = 1.6 * exp(-0.15 / 2 / 3.1415 * (255 - x)) + 0.8;
      } else if (x < -5) {
        x = 4 * exp(-0.15 / 2 / 3.1415 * (255 + x)) + 0.8;
      }

      jnd_factor[i][j] *= x;
    }
  }
#endif

  // save data of current frame
  // sum_last = sum_cur * picHeightY * picWidthY;
  sum_last = sum_cur;
  memcpy(img_last, img_input, sizeof(UINT8) * picWidthY * picHeightY);

  free(img_input);
  free(jnd_factor1);
  free(jnd_factor2);
  free(jnd_factor3);
  free(jnd_factor4);
  free(jnd_factor5);
  img_input = NULL;
  jnd_factor1 = NULL;
  jnd_factor2 = NULL;
  jnd_factor3 = NULL;
  jnd_factor4 = NULL;
  jnd_factor5 = NULL;

  // compute jnd in mi_size x mi_size
  int mi_Height, mi_Width;
  mi_Height =
      picHeightY % mi_size ? picHeightY / mi_size + 1 : picHeightY / mi_size;
  mi_Width =
      picWidthY % mi_size ? picWidthY / mi_size + 1 : picWidthY / mi_size;

  for (int i = 0; i < mi_Height; ++i) {
    for (int j = 0; j < mi_Width; ++j) {
      rdo_factor[i][j] = 0;

      int fromH = i * mi_size;
      int fromW = j * mi_size;
      int endH = (i == mi_Height - 1) ? picHeightY : fromH + mi_size;
      int endW = (j == mi_Width - 1) ? picWidthY : fromW + mi_size;
      int num_pixels = (endH - fromH) * (endW - fromW);
      for (int m = fromH; m < endH; ++m) {
        for (int n = fromW; n < endW; ++n) {
          rdo_factor[i][j] += jnd_factor[m][n];
        }
      }

      rdo_factor[i][j] = rdo_factor[i][j] / num_pixels;
    }
  }

  // scale
  for (int i = 0; i < mi_Height; ++i) {
    for (int j = 0; j < mi_Width; ++j) {
      rdo_factor[i][j] = pow(rdo_factor[i][j], 1.0 / 10);
    }
  }

#if 1 // compute jnd based weight for rdo
  double sum_of_factor = 0;
  for (int i = 0; i < mi_Height; ++i) {
    for (int j = 0; j < mi_Width; ++j) {
      if (rdo_factor[i][j] == 0) {
        continue;
      }
      sum_of_factor += pow(rdo_factor[i][j], 1.0 / -1.9);
    }
  }
  sum_of_factor /= (mi_Height * mi_Width);
  sum_of_factor = pow(sum_of_factor, -1.9);
  
  for (int i = 0; i < mi_Height; ++i) {
    for (int j = 0; j < mi_Width; ++j) {
      rdo_factor[i][j] = rdo_factor[i][j] / sum_of_factor;
    }
  }

  int above = 0;
  int below = 0;
  // mean value for jnd > 1 & jnd < 1
  double above_mean = 0;
  double below_mean = 0;

  for (int i = 0; i < mi_Height; ++i) {
    for (int j = 0; j < mi_Width; ++j) {
      if (rdo_factor[i][j] >= 1) {
        above += 1;
        above_mean += rdo_factor[i][j];
	  }
      if (rdo_factor[i][j] < 1) {
        below += 1;
		below_mean += rdo_factor[i][j];
	  }

    }
  }
  above_mean /= above;
  below_mean /= below;

  if ((above_mean - below_mean) > 0.11){
    for (int i = 0; i < mi_Height; ++i) {
      for (int j = 0; j < mi_Width; ++j) {
        if (rdo_factor[i][j] > 1) {
          rdo_factor[i][j] = pow(rdo_factor[i][j], 1.3);
        } else {
          rdo_factor[i][j] = pow(rdo_factor[i][j], 0.8);
          //rdo_factor[i][j] = 1;
        }
      }
    }
  } else if ((above_mean - below_mean) <= 0.11 &&
             (above_mean - below_mean) >= 0.06) {
    for (int i = 0; i < mi_Height; ++i) {
      for (int j = 0; j < mi_Width; ++j) {
        if (rdo_factor[i][j] > 1) {
          rdo_factor[i][j] = pow(rdo_factor[i][j], 1.5);
        } else {
          rdo_factor[i][j] = pow(rdo_factor[i][j], 1);
        }
      }
      
    }
  } else{ // above_mean - below_mean < 0.06
    for (int i = 0; i < mi_Height; ++i) {
      for (int j = 0; j < mi_Width; ++j) {
        if (rdo_factor[i][j] > 1) {
          rdo_factor[i][j] = pow(rdo_factor[i][j], 2.5);
        } else {
          rdo_factor[i][j] = pow(rdo_factor[i][j], 1.5);
        }
      }
    }
  }

  AV1_COMMON *const cm = &cpi->common;
  if (cm->frame_type == 0) { // KEY_FRAME
    for (int i = 0; i < mi_Height; ++i) {
      for (int j = 0; j < mi_Width; ++j) {
        rdo_factor[i][j] = 1;
      }
    }
  }
#endif
}

double jnd_calculation_sb(AV1_COMP *cpi, int mi_row, int mi_col,
                         BLOCK_SIZE bsize) {
  double jnd_factor = 0;

  int picWidthY = cpi->source->crop_widths[0];
  int picHeightY = cpi->source->crop_heights[0];
  int mi_Height, mi_Width;
  mi_Height =
      picHeightY % mi_size ? picHeightY / mi_size + 1 : picHeightY / mi_size;
  mi_Width =
      picWidthY % mi_size ? picWidthY / mi_size + 1 : picWidthY / mi_size;

  const BLOCK_SIZE plane_bsize = get_plane_block_size(bsize, 0, 0);
  const int bw = block_size_wide[plane_bsize];
  const int bh = block_size_high[plane_bsize];

  int sbLPelX = mi_col;
  int sbTPelY = mi_row;
  int sbRPelX = mi_col + bw / mi_size - 1;
  int sbBPelY = mi_row + bh / mi_size - 1;

  sbBPelY = min(sbBPelY, mi_Height - 1);
  sbRPelX = min(sbRPelX, mi_Width - 1);

  for (int h = sbTPelY; h <= sbBPelY; ++h) {
    for (int w = sbLPelX; w <= sbRPelX; ++w) {
      jnd_factor += rdo_factor[h][w];
    }
  }

  jnd_factor /= (sbBPelY - sbTPelY + 1) * (sbRPelX - sbLPelX + 1);

  return jnd_factor;
}

#endif
