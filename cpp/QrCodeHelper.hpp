
#ifndef _QRCODE_HELPER_HPP_
#define _QRCODE_HELPER_HPP_

#include "qrcodegen.hpp"

using qrcodegen::QrCode;

class QRCdodeHelper
{
public:
    static bool SaveBufferToBMP(unsigned char* imgBuffer, const char* pathWithFileName, const int width, const int height);
    static bool ZoomBMP(const char* srcBMP, const char* dstBMP, const int scale);
    static bool SaveQRDataToBMP(const QrCode& qrCode, const char* pathWithFileName, const int margin);


    // Description: Generate a QR Code symbol from the input text and save it as a bmp file and store it to the pathWithFileName 
    //
    // Input      : text - Normally is a url like "https://leica-geosystems.com/"
    //              pathWithFileName - The destination to store the generated bmp file
    //              scale - If the input size is 3, then a 29*29 QR code symbol would be generated, which is quite small,
    //                      so better to scale it 5 times, to get a 145*145 symbol
    //              size - Same as the "version" of QR code standard, minVersion = 1, maxVersion = 40, controls the complexity
    //                     of the QR code, larger version means bigger QR code symbol, but harder to scan
    //              correctionLevel - The error correction level in a QR Code symbol
    //              margin - The pixel(s) of the white space around each side of the original (before scaled) QR Code symbol
    // 
    // Return     : true    - The operation is successful
    //              false   - Failed
    static bool GetBMPFile(const char* text, const char* pathWithFileName, const int scale = 5,
        const int size = 3, const QrCode::Ecc correctionLevel = QrCode::MEDIUM, const int margin = 1);
};

#endif
