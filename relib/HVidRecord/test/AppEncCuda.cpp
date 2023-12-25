#include "hvidrec.h"
#include <istream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>

int main() {
    char szInFilePath[256] = "../../../test/aa.yuv";

    // Open input file
    std::ifstream fpIn(szInFilePath, std::ifstream::in | std::ifstream::binary);
    if (!fpIn) {
        std::ostringstream err;
        err << "Unable to open input file: " << szInFilePath << std::endl;
        throw std::invalid_argument(err.str());
    }
    auto inptr = hvid_record_open(0, 1280, 720, 50, 0, "./out");
    auto frameSize = hvid_record_get_vid_frame_buffsize(inptr);
    std::vector<char> frame;
    frame.resize(frameSize);
    while (true) {
        // Load the next frame from disk
        std::streamsize nRead = fpIn.read(frame.data(), frameSize).gcount();
        if (nRead == frameSize) {
            hvid_record_write_vid(inptr, frame.data(), nRead, false);
        } else {
            hvid_record_write_vid(inptr, frame.data(), nRead, true);
        }
        if (nRead != frameSize) {
            break;
        }
    }
    std::cout << hvid_record_get_vid_frame_count(inptr) << std::endl;
    hvid_record_close(inptr);
}
