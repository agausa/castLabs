//
//  main.cpp
//  castLabs
//
//  Created by Igor Dagayev on 12/28/20.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#define SWAP_INT32(x) (((x) >> 24) | (((x) & 0x00FF0000) >> 8) | (((x) & 0x0000FF00) << 8) | ((x) << 24))
#define MAX_BUFFER 64*1024  //keep it 64K
#define MAX_OUTPUT 1*1024   //keep it 1K

//
// base64 decoding code is not mine - took it from the Internet
//

static const std::string base64_chars =
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";

static inline bool is_base64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_decode(std::string const& encoded_string) {
  int in_len = encoded_string.size();
  int i = 0;
  int j = 0;
  int in_ = 0;
  unsigned char char_array_4[4], char_array_3[3];
  std::string ret;

  while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
    char_array_4[i++] = encoded_string[in_]; in_++;
    if (i ==4) {
      for (i = 0; i <4; i++)
        char_array_4[i] = base64_chars.find(char_array_4[i]);

      char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      for (i = 0; (i < 3); i++)
        ret += char_array_3[i];
      i = 0;
    }
  }

  if (i) {
    for (j = i; j <4; j++)
      char_array_4[j] = 0;

    for (j = 0; j <4; j++)
      char_array_4[j] = base64_chars.find(char_array_4[j]);

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

    for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
  }

  return ret;
}

//
// base64 decoding code is not mine - took it from the Internet
//


//
// find tag or attribute in the stream data and return new allocated string
// need to free allocated string in calling method
//

char* get_tag(char* data, char* start_tag, char* end_tag){
    char* start_pointer = strstr(data, start_tag);
    if(!start_pointer){
        return NULL;
    }
    long length = strlen(start_tag);
    char* end_pointer = strstr(start_pointer + length, end_tag);
    if(!end_pointer){
        return NULL;
    }
    
    long size = end_pointer - (start_pointer + length);
    
    char *tag = new char[size + 1];
    strncpy(tag, start_pointer + length, size);
    tag[size] = 0; // terminate
    return tag;
}

//
// will decode and save image to disk
//

void decode_and_save_image(char* data){
    // get image name
    char *name = get_tag(data, (char*)"xml:id=\"", (char*)"\"");
    if(name == NULL){
        std::cerr << "Not able to locate image name" << std::endl;
        return;
    }
 
    // get image type
    char *image_type = get_tag(data, (char*)"imagetype=\"", (char*)"\"");
    if(image_type == NULL){
        std::cerr << "Not able to locate image type" << std::endl;
        return;
    }
    
    std::cout << "File name is: " << name << "." << image_type << std::endl;
    
    // get the image body
    char *image_body = get_tag(data, (char*)"Base64\">", (char*)"</");
    std::string decoded_body = base64_decode(image_body);
    
    // write into the file
    std::string file_name = std::string(name) + std::string(".") + image_type;
    
    std::fstream imagefile;
    imagefile.open(file_name, std::ios_base::out | std::ios_base::binary);
    
    if(!imagefile.is_open()){
        std::cerr << "Can't create file: " << file_name << std::endl;
    }else {
        //for(int i = 0; i < decoded_size; i++)
        imagefile << decoded_body;
        imagefile.close();
    }
    
    // clean up
    delete [] name;
    delete [] image_type;
    delete [] image_body;
}

//
// will parse the buffer and locate the image tag -> smpte:image
//

void parse_mdat(char* data, int size){
    // walk throuth and decode images if we have them
    long pos = 0;
    long image_size = 0;
    char* end_pointer = NULL;
    while(pos < size){
        // lookup for tags <smpte:image
        char* start_pointer = strstr(data + pos, "<smpte:image");
        if(!start_pointer) break;

        // find the end pointer
        end_pointer = strstr(data + pos, "</smpte:image>");
        if(!end_pointer) break;

        image_size = end_pointer - start_pointer;
        std::cout << "The image size in XML: " << image_size << " bytes" << std::endl;
        decode_and_save_image(start_pointer);
        pos = end_pointer - data + std::string("</smpte:image>").size();
    }
}

//
// Will use this function to parse stream
//

void parse_buffer(char *data, int size){
    int pos = 0;
    
    uint box_size;
    char box_type[5];
    
    // terminate
    box_type[4] = 0;
    
    while(pos < size){
        // get size
        box_size = *(uint*)(data + pos);
        pos += 4;

        // size is big-endian - swap it
        box_size = SWAP_INT32(box_size);
        
        // get box type
        strncpy(box_type, data + pos, 4);
        pos += 4;
        
        // report
        printf("Found box of type %s and size %d\n", box_type, box_size);
        
        if(strcmp((char*)box_type, "traf") == 0){
            parse_buffer(data + pos, box_size - 4 - 4);
        }
        
        pos += box_size - 4 - 4;
    }
}

//
// M A I N
//

int main(int argc, const char * argv[]) {
    
    std::fstream mp4file;
    uint box_size;
    char box_type[5];
    
    // terminate
    box_type[4] = 0;
    
    if(argc < 2){
        std::cout << "Usage: castLabs filename.mp4\n";
        return 1;
    }
    
    // open file for readonly in binary mode
    mp4file.open(argv[1], std::ios_base::in | std::ios_base::binary);
    
    if(!mp4file.is_open()){
        std::cout << "Can't open file: ";
        std::cout << argv[1] << "\n";
        return 2;
    }

    std::cout << "Successfully loaded file ";
    std::cout << argv[1] << "\n";

    // get file size
    long pos = 0;
    long file_size = (long)mp4file.tellg();
    mp4file.seekg(0, std::ios::end);
    file_size = (long)mp4file.tellg() - file_size;
    
    // move pointer back
    mp4file.seekg(0, std::ios::beg);
    
    std::cout << "File size is: " << file_size << std::endl;
        
    while(pos < file_size){
        // get size
        mp4file.read((char*)&box_size, sizeof(box_size));

        // size is big-endian - swap it
        box_size = SWAP_INT32(box_size);
        
        // get box type
        mp4file.read((char*)&box_type, 4);
        
        // report
        printf("Found box of type %s and size %d\n", box_type, box_size);
        
        if(strcmp((char*)box_type, "moof") == 0){
            // allocate and read the chunk
            if(box_size < MAX_BUFFER){
                char* buffer = new char[box_size];
                if(buffer){
                    for(int i = 0; i <= box_size; i++)
                        mp4file >> buffer[i];

                    parse_buffer(buffer, box_size - 4 - 4);
                    
                    delete [] buffer;
                }
            }
        }
        else if(strcmp((char*)box_type, "mdat") == 0){
            if(box_size < MAX_BUFFER){
                char* buffer = new char[box_size];
                if(buffer){
                    for(int i = 0; i <= box_size; i++)
                        mp4file >> buffer[i];
                    
                    // lookup for images
                    parse_mdat(buffer, box_size);

                    // output
                    std::cout << "Content of mdat block is: ";
                    int count = (box_size < MAX_OUTPUT)?box_size:MAX_OUTPUT;
                    for(int i = 0; i <= count; i++){
                        std::cout << buffer[i];
                    }
                    delete [] buffer;
                }
            }
        }
        
        pos += box_size;
        // move pointer in stream.
        mp4file.seekg(pos, std::ios_base::beg);
    }
    
    // close and out
    mp4file.close();
    
    std::cout << std::endl << "We done!" << std::endl;
    return 0;
}

