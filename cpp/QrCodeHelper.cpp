// ====================================================================================================================
//
// Project  : Sea the Stars
//
// Component: QR Code generator helper
//
//
// Summary  : QR Code generator helper
//
// --------------------------------------------------------------------------------------------------------------------
//
// Copyright 2022 by Leica Geosystems AG, Heerbrugg
//
// ====================================================================================================================

#define _CRT_SECURE_NO_DEPRECATE
#pragma pack(1)// need this, otherwise cant get correct result of sizeof
#include <iostream>
#include "QRCodeHelper.hpp"

using qrcodegen::QrSegment;

typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef long    LONG;
typedef struct {
    WORD    bfType;
    DWORD   bfSize;
    WORD    bfReserved1;
    WORD    bfReserved2;
    DWORD   bfOffBits;
} BITMAPFILEHEADER;

typedef struct {
    DWORD      biSize;
    LONG       biWidth;
    LONG       biHeight;
    WORD       biPlanes;
    WORD       biBitCount;
    DWORD      biCompression;
    DWORD      biSizeImage;
    LONG       biXPelsPerMeter;
    LONG       biYPelsPerMeter;
    DWORD      biClrUsed;
    DWORD      biClrImportant;
} BITMAPINFOHEADER;


//=============================================================
//
bool QRCdodeHelper::SaveBufferToBMP(unsigned char* imgBuffer, const char* pathWithFileName, const int width, const int height)
{
    if (NULL == imgBuffer || NULL == pathWithFileName)
    {
        return false;
    }
    
    FILE* file = fopen(pathWithFileName, "wb");
    if (NULL == file)
    {
        return false;
    }
    
    const int biCount = 24;//true color
    const int lineByte = (width * biCount / 8 + 3) / 4 * 4; //needs to be devided by 4

    BITMAPFILEHEADER bitMapFileHeader;
    bitMapFileHeader.bfType = 0x4D42;
    bitMapFileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + lineByte * height;
    bitMapFileHeader.bfReserved1 = 0;
    bitMapFileHeader.bfReserved2 = 0;
    bitMapFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    BITMAPINFOHEADER bitMapInfoHeader;
    bitMapInfoHeader.biBitCount = biCount;
    bitMapInfoHeader.biClrImportant = 0;
    bitMapInfoHeader.biClrUsed = 0;
    bitMapInfoHeader.biCompression = 0;
    bitMapInfoHeader.biHeight = height;
    bitMapInfoHeader.biPlanes = 1;
    bitMapInfoHeader.biSize = 40;
    bitMapInfoHeader.biSizeImage = lineByte * height;
    bitMapInfoHeader.biWidth = width;
    bitMapInfoHeader.biXPelsPerMeter = 0;
    bitMapInfoHeader.biYPelsPerMeter = 0;

    fwrite(&bitMapFileHeader, sizeof(BITMAPFILEHEADER), 1, file);
    fwrite(&bitMapInfoHeader, sizeof(BITMAPINFOHEADER), 1, file);
    fwrite(imgBuffer, height * lineByte, 1, file);
    fclose(file);
    
    return true;
}

bool QRCdodeHelper::ZoomBMP(const char* srcBMP, const char* dstBMP, const int scale)
{
    if (scale < 2)
    {
        return true;
    }

    FILE* srcfile = fopen(srcBMP, "rb");
    if (NULL == srcfile)
    {
        return false;
    }

    fseek(srcfile, sizeof(BITMAPFILEHEADER), 0);

    BITMAPINFOHEADER head;
    fread(&head, sizeof(BITMAPINFOHEADER), 1, srcfile);
    const int bmpWidth = head.biWidth;
    const int bmpHeight = head.biHeight;
    const int biBitCount = head.biBitCount;
    const int lineByte = (bmpWidth * biBitCount / 8 + 3) / 4 * 4;

    unsigned char* srcBuf = new unsigned char[lineByte * bmpHeight];
    fread(srcBuf, 1, lineByte * bmpHeight, srcfile);
    fclose(srcfile);

    const int newBmpWidth = bmpWidth * scale;
    const int newBmpHeight = bmpHeight * scale;
    const int newLineByte = (newBmpWidth * biBitCount / 8 + 3) / 4 * 4;
    unsigned char* newBuf = new unsigned char[newLineByte * newBmpHeight];
    memset(newBuf, 255, newLineByte * newBmpHeight);//set to white by default
    
    for (int i = 0; i < newBmpWidth; ++i)
    {
        for (int j = 0; j < newBmpHeight; ++j)
        {
            int oldX = i / scale;
            int oldY = j / scale;
            for (int k = 0; k < 3; ++k)
            {
                if ((oldX >= 0) && (oldX < bmpWidth) && (oldY >= 0) && (oldY < newBmpHeight))
                {
                    *(newBuf + i * newLineByte + j * 3 + k) = *(srcBuf + oldX * lineByte + oldY * 3 + k);
                }
            }
        }
    }

    SaveBufferToBMP(newBuf, dstBMP, newBmpWidth, newBmpHeight);
    delete[] srcBuf;
    delete[] newBuf;
    return true;
}

bool QRCdodeHelper::SaveQRDataToBMP(const QrCode& qrCode, const char* pathWithFileName, const int margin)
{
    FILE* file = fopen(pathWithFileName, "wb");
    if (NULL == file)
    {
        return false;
    }

    const int width = qrCode.getSize() + margin * 2;//pixel of white margin for each side
    const int height = qrCode.getSize() + margin * 2;

    const int biCount = 24;//true color
    const int lineByte = (width * biCount / 8 + 3) / 4 * 4; //needs to be devided by 4

    BITMAPFILEHEADER bitMapFileHeader;
    bitMapFileHeader.bfType = 0x4D42;
    bitMapFileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + lineByte * height;
    bitMapFileHeader.bfReserved1 = 0;
    bitMapFileHeader.bfReserved2 = 0;
    bitMapFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    BITMAPINFOHEADER bitMapInfoHeader;
    bitMapInfoHeader.biBitCount = biCount;
    bitMapInfoHeader.biClrImportant = 0;
    bitMapInfoHeader.biClrUsed = 0;
    bitMapInfoHeader.biCompression = 0;
    bitMapInfoHeader.biHeight = height;
    bitMapInfoHeader.biPlanes = 1;
    bitMapInfoHeader.biSize = 40;
    bitMapInfoHeader.biSizeImage = lineByte * height;
    bitMapInfoHeader.biWidth = width;
    bitMapInfoHeader.biXPelsPerMeter = 0;
    bitMapInfoHeader.biYPelsPerMeter = 0;

    fwrite(&bitMapFileHeader, sizeof(BITMAPFILEHEADER), 1, file);
    fwrite(&bitMapInfoHeader, sizeof(BITMAPINFOHEADER), 1, file);

    unsigned char* data = new unsigned char[lineByte * height];
    memset(data, 255, lineByte * height);//set to white by default

    for (int i = 0; i < width; ++i)
    {
        for (int j = 0; j < height; ++j)
        {
            qrCode.getModule(i, j) ? printf("%c%c", 219, 219) : printf("  ");
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    for (int i = 0; i < width; ++i)
    {
        for (int j = 0; j < height; ++j)
        {
            if (qrCode.getModule(i, j))
            {
                *(data + lineByte * (height - 1 - (j + margin)) + 3 * (i + margin)) = 0; // bmp data is upside down, so need to use (height - 1 - j)
                *(data + lineByte * (height - 1 - (j + margin)) + 3 * (i + margin) + 1) = 0;
                *(data + lineByte * (height - 1 - (j + margin)) + 3 * (i + margin) + 2) = 0;
            }
        }
    }

    fwrite(data, sizeof(unsigned char), lineByte * height, file);
    fclose(file);
    delete[] data;
    data = NULL;

    return true;
}

bool QRCdodeHelper::GetBMPFile(const char* text, const char* pathWithFileName, const int scale, 
    const int size, const QrCode::Ecc correctionLevel, const int margin)
{
    if (NULL == text || NULL == pathWithFileName)
    {
        return false;
    }

    const std::vector<QrSegment> segs = QrSegment::makeSegments(text);
    const QrCode qr = QrCode::encodeSegments(segs, correctionLevel, size, size, 7, false);

    if (!SaveQRDataToBMP(qr, pathWithFileName, margin))
    {
        return false;
    }

    return ZoomBMP(pathWithFileName, pathWithFileName, scale);
}
