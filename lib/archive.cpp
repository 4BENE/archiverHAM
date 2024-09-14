#include "archive.h"
#include <fstream>
#include <iostream>


int power(int num, int pow) {
    int result = 1;
    for (int i = 0; i < pow; i++) {
        result = result * num;
    }
    return result;
}

std::bitset<ByteSize> DeCoderHam(std::bitset<kReNewSize>& bits) {
    std::bitset<ByteSize> de_code;
    int control_bits = 0;
    std::bitset<kReNewSize> check_bits = bits;
    int k = 0;
    while (k < kReNewSize) {
        check_bits[k] = 0;
        k = 2 * k + 1;
    }

    for (int i = 0; i < kReNewSize; i++) {
        if (check_bits[i] == 1) {
            std::bitset<ByteSize> local = Eight_Bites(i + 1);
            for (int j = 0; j < ByteSize; j++) {
                if (local[j] == 1) {
                    check_bits[power(2, 7 - j) - 1] = (check_bits[power(2, 7 - j) - 1] + 1) % 2;
                }
            }
        }
    }


    int cont_sum = 0;
    int var_for_check = 0;
    std::vector<int> bad_bits;

    while (var_for_check < kReNewSize) {
        if (check_bits[var_for_check] != bits[var_for_check]) {
            bad_bits.push_back(var_for_check);
            cont_sum += var_for_check + 1;
        }
        var_for_check = var_for_check * 2 + 1;
    }

    if (cont_sum >= kReNewSize){
        std::cout<<"More than 1 error";
        return 0;
    }
    else {
        if (!bad_bits.empty()) {
            for (int i = 0; i < bad_bits.size(); i++) {
                bits[bad_bits[i]] = (bits[bad_bits[i]] + 1) % 2;
            }
            bits[cont_sum - 1] = (bits[cont_sum - 1] + 1) % 2;
        }

        control_bits = 0;
        int digit = 0;
        int pos_in_bitset = 0;
        while (digit < kReNewSize) {
            if (digit == control_bits) {
                control_bits = control_bits * 2 + 1;
            } else {
                de_code[pos_in_bitset] = bits[digit];
                pos_in_bitset++;
            }
            digit++;
        }
    }
    return de_code;
}

std::bitset<kReNewSize> CoderHam(uint8_t digit) {
    std::bitset<kReNewSize> bits;
    std::bitset<ByteSize> char_bits = Eight_Bites(digit);
    int control_bits = ByteSize - 1;
    int counter = 7;
    for (int i = kReNewSize - 1; i >= 0; i--) {
        if (i != control_bits) {
            bits[i] = char_bits[counter];
            counter--;
        } else {
            control_bits = control_bits / 2;
        }
    }

    for (int i = 0; i < kReNewSize; i++) {
        if (bits[i] == 1) {
            std::bitset<ByteSize> local = Eight_Bites(i + 1);
            for (int j = 0; j < ByteSize; j++) {
                if (local[j] == 1) {
                    bits[power(2, 7 - j) - 1] = (bits[power(2, 7 - j) - 1] + 1) % 2;
                }
            }
        }
    }
    return bits;
}

std::bitset<ByteSize> Eight_Bites(int byte) {
    std::bitset<ByteSize> local;
    int digit = byte;
    for (int i = ByteSize - 1; i >= 0; i--) {
        local[i] = digit % 2;
        digit = digit / 2;
    }
    return local;
}

std::bitset<ByteSize> Eight_Bites(char byte) {
    std::bitset<ByteSize> local;
    int digit = int(byte);
    for (int i = ByteSize - 1; i >= 0; i--) {
        local[i] = digit % 2;
        digit = digit / 2;
    }
    return local;
}


void MoveByte(uint8_t& byte_to_move, std::bitset<8> bits) {
    for (int i = 0; i < ByteSize; i++) {
        byte_to_move = byte_to_move << 1 | bits[i];
    }
}


void Archive::GetFile(const std::string& file_name) {
    bool is_find = false;
    std::ifstream in(archive_path, std::ios::binary);
    std::ofstream out(file_name, std::ios::binary);
    char digit_char;
    uint8_t digit;
    std::string name;
    while (!is_find && !in.eof()) {
        while (in.get(digit_char)) {
            std::bitset<ByteSize> size_file_bitset;
            std::bitset<kReNewSize> bits_digit_char;
            size_file_bitset = Eight_Bites(digit_char);
            for (int i = 0; i < ByteSize; i++) {
                bits_digit_char[i] = size_file_bitset[i];
            }

            in.get(digit_char);
            uint8_t digit_uint = digit_char;
            for (int i = ByteSize; i < kReNewSize; i++) {
                bits_digit_char[i] = size_file_bitset[i];
            }

            size_file_bitset = DeCoderHam(bits_digit_char);
            MoveByte(digit_uint, size_file_bitset);

            int size_file = digit_uint;


            int count_let = 0;
            std::bitset<kReNewSize> before_de_code_name;

            while (count_let < size_file) {
                in.get(digit_char);
                std::bitset<ByteSize> buffer;
                std::bitset<ByteSize> de_code;
                in.get(digit_char);
                digit = char(digit_char);
                buffer = Eight_Bites(digit);
                if (count_let % 3 == 0) {
                    for (int i = 0; i < ByteSize; i++) {
                        before_de_code_name[i] = buffer[i];
                    }
                } else if (count_let % 3 == 1) {
                    for (int i = ByteSize; i < kReNewSize; i++) {
                        before_de_code_name[i] = buffer[i - ByteSize];
                    }
                    de_code = DeCoderHam(before_de_code_name);
                    MoveByte(digit, de_code);
                    name += std::to_string(digit);
                    for (int i = 0; i < kReNewSize - ByteSize; i++) {
                        before_de_code_name[i] = buffer[kReNewSize - ByteSize + i];
                    }
                } else if (count_let % 3 == 2) {
                    for (int i = kReNewSize - ByteSize; i < kReNewSize; i++) {
                        before_de_code_name[i] = buffer[i - kReNewSize + ByteSize];
                    }
                    de_code = DeCoderHam(before_de_code_name);
                    MoveByte(digit, de_code);
                    name += std::to_string(digit);
                }
                count_let++;
            }


            int counter = 1;
            size_file = 0;
            for (int j = 0; j < 6; j++) {
                in.get(digit_char);
                std::bitset<ByteSize> buffer;
                std::bitset<ByteSize> de_code;
                in.get(digit_char);
                digit = char(digit_char);
                buffer = Eight_Bites(digit);
                if (count_let % 3 == 0) {
                    for (int i = 0; i < ByteSize; i++) {
                        before_de_code_name[i] = buffer[i];
                    }
                } else if (count_let % 3 == 1) {
                    for (int i = ByteSize; i < kReNewSize; i++) {
                        before_de_code_name[i] = buffer[i - ByteSize];
                    }
                    de_code = DeCoderHam(before_de_code_name);
                    MoveByte(digit, de_code);
                    size_file += int(digit_char) * counter;
                    for (int i = 0; i < kReNewSize - ByteSize; i++) {
                        before_de_code_name[i] = buffer[kReNewSize - ByteSize + i];
                    }
                } else if (count_let % 3 == 2) {
                    for (int i = kReNewSize - ByteSize; i < kReNewSize; i++) {
                        before_de_code_name[i] = buffer[i - kReNewSize + ByteSize];
                    }
                    de_code = DeCoderHam(before_de_code_name);
                    MoveByte(digit, de_code);
                    size_file += int(digit_char) * counter;
                }

                counter = counter * 256;
            }

            if (name != file_name) {
                in.seekg(std::ios::cur + size_file);
            } else {
                is_find = true;
                counter = 0;
                std::bitset<kReNewSize> before_de_code;
                while (counter < size_file) {
                    std::bitset<ByteSize> buffer;
                    std::bitset<ByteSize> de_code;
                    in.get(digit_char);
                    digit = char(digit_char);
                    buffer = Eight_Bites(digit);
                    if (counter % 3 == 0) {
                        for (int i = 0; i < ByteSize; i++) {
                            before_de_code[i] = buffer[i];
                        }
                    } else if (counter % 3 == 1) {
                        for (int i = ByteSize; i < kReNewSize; i++) {
                            before_de_code[i] = buffer[i - ByteSize];
                        }
                        de_code = DeCoderHam(before_de_code);
                        MoveByte(digit, de_code);
                        out << digit;
                        for (int i = 0; i < kReNewSize - ByteSize; i++) {
                            before_de_code[i] = buffer[kReNewSize - ByteSize + i];
                        }
                    } else if (counter % 3 == 2) {
                        for (int i = kReNewSize - ByteSize; i < kReNewSize; i++) {
                            before_de_code[i] = buffer[i - kReNewSize + ByteSize];
                        }
                        de_code = DeCoderHam(before_de_code);
                        MoveByte(digit, de_code);
                        out << digit;
                    }
                    counter++;
                }
                break;
            }
        }
    }
}

std::string file_size(int file_size) {
    int counter = 0;
    int counter_for_char = 0;
    int digit = 0;
    int pow = 1;
    std::string answer;
    while (counter_for_char < 4) {
        if (counter == 8) {
            answer += char(digit);
            counter_for_char++;
            counter = 0;
            pow = 1;
            digit = 0;
        }
        digit = digit + (file_size % 2) * pow;
        pow = pow * 2;
        file_size = file_size / 2;
        counter++;
    }
    return answer;
}

void Archive::IncludeFile(const std::string& file_name) {
    char digit_char;
    uint8_t digit;
    //открываем поток
    std::ifstream in(file_name, std::ios::binary);
    std::ofstream out(this->archive_path, std::ios::app | std::ios::binary);
    File_Header new_file;
    //получаем данные хеддера файла
    in.seekg(0, std::ios::end);
    new_file.file_name = file_name;


    if (file_name.size() % 2 != 0) {
        new_file.file_name_size = int(file_name.size() * (float(kReNewSize) / ByteSize) + 1);
    } else {
        new_file.file_name_size = int(file_name.size() * (float(kReNewSize) / ByteSize));
    }


    if (in.tellg() % 2 != 0) {
        new_file.file_size = int(in.tellg() * (float(kReNewSize) / ByteSize) + 1);
    } else {
        new_file.file_size = int(in.tellg() * (float(kReNewSize) / ByteSize));
    }

    files.push_back(new_file);
    in.seekg(0);


    std::bitset<ByteSize> temple_buffer;
    std::bitset<kReNewSize> code_buffer;
    uint32_t pos_buffer = 0;

    std::string code_file_size;
    code_file_size = file_size(new_file.file_size);



    std::bitset<12> code_size;
    std::bitset<8> f_code_size;
    code_size = CoderHam(uint8_t(new_file.file_name_size));
    for (int i = 0; i < 8; i++) {
        f_code_size[i] = code_size[i];
    }
    uint8_t temple;
    MoveByte(temple, f_code_size);
    out << temple;
    for (int i = 0; i < kReNewSize - ByteSize; i++) {
        f_code_size[i] = code_size[ByteSize + i];
    }
    for (int i = kReNewSize - ByteSize; i < ByteSize; i++) {
        f_code_size[i] = 0;
    }
    MoveByte(temple, f_code_size);
    out << temple;



    //кодируем имя
    int j = 0;
    while (j < file_name.size()) {
        digit_char = file_name[j];
        digit = digit_char;
        code_buffer = CoderHam(digit);
        if (j % 2 == 0) {
            for (int i = 0; i < ByteSize; ++i) {
                temple_buffer[i] = code_buffer[i];
            }
            MoveByte(digit, temple_buffer);
            out << digit;
            for (int i = 0; i < kReNewSize - ByteSize; ++i) {
                temple_buffer[i] = code_buffer[i + ByteSize];
            }
        } else {
            for (int i = 0; i < kReNewSize - ByteSize; ++i) {
                temple_buffer[i + kReNewSize - ByteSize] = code_buffer[i];
            }
            MoveByte(digit, temple_buffer);
            out << digit;
            for (int i = 0; i < ByteSize; ++i) {
                temple_buffer[i] = code_buffer[i + kReNewSize - ByteSize];
            }
            MoveByte(digit, temple_buffer);
            out << digit;
        }
        j++;
    }

    if (pos_buffer % 2 == 1) {
        for (int i = kReNewSize - ByteSize; i < ByteSize; i++) {
            temple_buffer[i] = 0;
        }
        MoveByte(digit, temple_buffer);
        out << digit;
    }

    out << code_file_size[0] << code_file_size[1] << code_file_size[2] << code_file_size[3];
    //кодируем размер файла
    j = 0;
    while (j < 4) {
        digit_char = file_name[j];
        digit = digit_char;
        code_buffer = CoderHam(digit);
        if (j % 2 == 0) {
            for (int i = 0; i < ByteSize; ++i) {
                temple_buffer[i] = code_buffer[i];
            }
            MoveByte(digit, temple_buffer);
            out << digit;
            for (int i = 0; i < kReNewSize - ByteSize; ++i) {
                temple_buffer[i] = code_buffer[i + ByteSize];
            }
        } else {
            for (int i = 0; i < kReNewSize - ByteSize; ++i) {
                temple_buffer[i + kReNewSize - ByteSize] = code_buffer[i];
            }
            MoveByte(digit, temple_buffer);
            out << digit;
            for (int i = 0; i < ByteSize; ++i) {
                temple_buffer[i] = code_buffer[i + kReNewSize - ByteSize];
            }
            MoveByte(digit, temple_buffer);
            out << digit;
        }
        j++;
    }



    //Кодируем сам текст
    while (in.get(digit_char) && pos_buffer < new_file.file_size) {
        digit = digit_char;
        code_buffer = CoderHam(digit);
        if (pos_buffer % 2 == 0) {
            for (int i = 0; i < ByteSize; ++i) {
                temple_buffer[i] = code_buffer[i];
            }
            MoveByte(digit, temple_buffer);
            out << digit;
            for (int i = 0; i < kReNewSize - ByteSize; ++i) {
                temple_buffer[i] = code_buffer[i + ByteSize];
            }
        } else {
            for (int i = 0; i < kReNewSize - ByteSize; ++i) {
                temple_buffer[i + kReNewSize - ByteSize] = code_buffer[i];
            }
            MoveByte(digit, temple_buffer);
            out << digit;
            for (int i = 0; i < ByteSize; ++i) {
                temple_buffer[i] = code_buffer[i + kReNewSize - ByteSize];
            }

            MoveByte(digit, temple_buffer);
            out << digit;
        }
        pos_buffer++;
    }

    if (pos_buffer % 2 == 1) {
        for (int i = kReNewSize - ByteSize; i < ByteSize; i++) {
            temple_buffer[i] = 0;
        }
        MoveByte(digit, temple_buffer);
        out << digit;
    }
    in.close();
}


void Archive::Make_One_Archive(const Archive& archive1, const Archive& archive2) {
    Archive local_archive;
    std::ifstream in1(archive1.archive_path, std::ios::binary);
    std::ifstream in2(archive2.archive_path, std::ios::binary);
    std::ofstream out(this->archive_path, std::ios::binary);
    char digit;
    while (in1.get(digit)) {
        out << digit;
    }
    while (in2.get(digit)) {
        out << digit;
    }
    for (int i = 0; i < archive1.files.size(); i++) {
        this->files.push_back(archive1.files[i]);
    }
    for (int i = 0; i < archive2.files.size(); i++) {
        this->files.push_back(archive2.files[i]);
    }
    in1.close();
    in2.close();
    remove(archive1.archive_path.c_str());
    remove(archive2.archive_path.c_str());
    out.close();
}

void Archive::DeleteFile(const std::string& file_name) {
    Archive local_archive;
    local_archive.archive_path = this->archive_path;
    local_archive.files = this->files;
    std::ifstream in(archive_path, std::ios::binary);
    std::ofstream out(archive_path, std::ios::binary);
    char digit_char;
    uint8_t digit;
    bool is_find = false;
    while (!in.eof()) {
        std::string buffer_for_name;
        std::string buffer_for_size_name;
        if (!is_find) {
            in.get(digit_char);
            digit = digit_char;

            //декодируем длину названия
            std::bitset<ByteSize> size_file_bitset;
            std::bitset<kReNewSize> bits_digit_char;
            size_file_bitset = Eight_Bites(digit_char);
            buffer_for_size_name += digit_char;
            for (int i = 0; i < ByteSize; i++) {
                bits_digit_char[i] = size_file_bitset[i];
            }

            in.get(digit_char);
            buffer_for_size_name += digit_char;
            digit = digit_char;
            for (int i = ByteSize; i < kReNewSize; i++) {
                bits_digit_char[i] = size_file_bitset[i];
            }

            size_file_bitset = DeCoderHam(bits_digit_char);
            MoveByte(digit, size_file_bitset);

            int size_file = digit;


            int count_let = 0;
            std::bitset<kReNewSize> before_de_code_name;
            std::string name;
            while (count_let < size_file) {
                in.get(digit_char);
                buffer_for_name += digit_char;
                std::bitset<ByteSize> buffer;
                std::bitset<ByteSize> de_code;
                in.get(digit_char);
                digit = char(digit_char);
                buffer = Eight_Bites(digit);
                if (count_let % 3 == 0) {
                    for (int i = 0; i < ByteSize; i++) {
                        before_de_code_name[i] = buffer[i];
                    }
                } else if (count_let % 3 == 1) {
                    for (int i = ByteSize; i < kReNewSize; i++) {
                        before_de_code_name[i] = buffer[i - ByteSize];
                    }
                    de_code = DeCoderHam(before_de_code_name);
                    MoveByte(digit, de_code);
                    name += std::to_string(digit);
                    for (int i = 0; i < kReNewSize - ByteSize; i++) {
                        before_de_code_name[i] = buffer[kReNewSize - ByteSize + i];
                    }
                } else if (count_let % 3 == 2) {
                    for (int i = kReNewSize - ByteSize; i < kReNewSize; i++) {
                        before_de_code_name[i] = buffer[i - kReNewSize + ByteSize];
                    }
                    de_code = DeCoderHam(before_de_code_name);
                    MoveByte(digit, de_code);
                    name += std::to_string(digit);
                }
                count_let++;
            }


            int counter = 1;
            size_file = 0;
            for (int j = 0; j < 6; j++) {
                in.get(digit_char);
                std::bitset<ByteSize> buffer;
                std::bitset<ByteSize> de_code;
                in.get(digit_char);
                digit = char(digit_char);
                buffer = Eight_Bites(digit);
                if (count_let % 3 == 0) {
                    for (int i = 0; i < ByteSize; i++) {
                        before_de_code_name[i] = buffer[i];
                    }
                } else if (count_let % 3 == 1) {
                    for (int i = ByteSize; i < kReNewSize; i++) {
                        before_de_code_name[i] = buffer[i - ByteSize];
                    }
                    de_code = DeCoderHam(before_de_code_name);
                    MoveByte(digit, de_code);
                    size_file += int(digit_char) * counter;
                    for (int i = 0; i < kReNewSize - ByteSize; i++) {
                        before_de_code_name[i] = buffer[kReNewSize - ByteSize + i];
                    }
                } else if (count_let % 3 == 2) {
                    for (int i = kReNewSize - ByteSize; i < kReNewSize; i++) {
                        before_de_code_name[i] = buffer[i - kReNewSize + ByteSize];
                    }
                    de_code = DeCoderHam(before_de_code_name);
                    MoveByte(digit, de_code);
                    size_file += int(digit_char) * counter;
                }

                counter = counter * 256;
            }

            if (name == file_name) {
                is_find = true;
                in.seekg(std::ios::cur + size_file);
            } else {
                out << buffer_for_size_name << buffer_for_name;
                for (int i = 0; i < size_file; i++) {
                    in.get(digit_char);
                    out << digit;
                }
            }
        } else {
            in.get(digit_char);
            out << digit;
        }
    }
}

void Archive::Show_List() {
    std::ifstream in(archive_path);
    char digit_char;
    uint8_t digit;
    while (in.get(digit_char)) {
        std::bitset<ByteSize> size_file_bitset;
        std::bitset<kReNewSize> bits_digit_char;
        size_file_bitset = Eight_Bites(digit_char);
        for (int i = 0; i < ByteSize; i++) {
            bits_digit_char[i] = size_file_bitset[i];
        }

        in.get(digit_char);
        digit = digit_char;
        for (int i = ByteSize; i < kReNewSize; i++) {
            bits_digit_char[i] = size_file_bitset[i];
        }
        size_file_bitset = DeCoderHam(bits_digit_char);
        MoveByte(digit, size_file_bitset);
        int size_file = digit;


        int count_let = 0;
        std::bitset<kReNewSize> before_de_code_name;
        std::string name;
        while (count_let < size_file) {
            in.get(digit_char);
            std::bitset<ByteSize> buffer;
            std::bitset<ByteSize> de_code;
            in.get(digit_char);
            digit = char(digit_char);
            buffer = Eight_Bites(digit);
            if (count_let % 3 == 0) {
                for (int i = 0; i < ByteSize; i++) {
                    before_de_code_name[i] = buffer[i];
                }
            } else if (count_let % 3 == 1) {
                for (int i = ByteSize; i < kReNewSize; i++) {
                    before_de_code_name[i] = buffer[i - ByteSize];
                }
                de_code = DeCoderHam(before_de_code_name);
                MoveByte(digit, de_code);
                name += std::to_string(digit);
                for (int i = 0; i < kReNewSize - ByteSize; i++) {
                    before_de_code_name[i] = buffer[kReNewSize - ByteSize + i];
                }
            } else if (count_let % 3 == 2) {
                for (int i = kReNewSize - ByteSize; i < kReNewSize; i++) {
                    before_de_code_name[i] = buffer[i - kReNewSize + ByteSize];
                }
                de_code = DeCoderHam(before_de_code_name);
                MoveByte(digit, de_code);
                name += std::to_string(digit);
            }
            count_let++;
        }


        int counter = 1;
        size_file = 0;
        for (int j = 0; j < 6; j++) {
            in.get(digit_char);
            std::bitset<ByteSize> buffer;
            std::bitset<ByteSize> de_code;
            in.get(digit_char);
            digit = char(digit_char);
            buffer = Eight_Bites(digit);
            if (count_let % 3 == 0) {
                for (int i = 0; i < ByteSize; i++) {
                    before_de_code_name[i] = buffer[i];
                }
            } else if (count_let % 3 == 1) {
                for (int i = ByteSize; i < kReNewSize; i++) {
                    before_de_code_name[i] = buffer[i - ByteSize];
                }
                de_code = DeCoderHam(before_de_code_name);
                MoveByte(digit, de_code);
                size_file += int(digit_char) * counter;
                for (int i = 0; i < kReNewSize - ByteSize; i++) {
                    before_de_code_name[i] = buffer[kReNewSize - ByteSize + i];
                }
            } else if (count_let % 3 == 2) {
                for (int i = kReNewSize - ByteSize; i < kReNewSize; i++) {
                    before_de_code_name[i] = buffer[i - kReNewSize + ByteSize];
                }
                de_code = DeCoderHam(before_de_code_name);
                MoveByte(digit, de_code);
                size_file += int(digit_char) * counter;
            }

            counter = counter * 256;
        }
        std::cout<<name;
        in.seekg(std::ios::cur + size_file);
    }
}