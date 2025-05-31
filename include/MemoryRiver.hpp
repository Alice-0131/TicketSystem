#ifndef MEMORYRIVER_HPP
#define MEMORYRIVER_HPP

template<class T, int info_len = 2>
class MemoryRiver {
private:
  std::fstream file;
  std::string file_name;
  int sizeofT = sizeof(T);
public:
  MemoryRiver() = default;

  MemoryRiver(const std::string& file_name) : file_name(file_name) {
    initialise();
  }

  void initialise() {
    file.open(file_name, std::ios::out | std::ios::in | std::ios::binary);
    if (!file) {
      file.open(file_name, std::ios::out | std::ios::binary);
      int tmp = 0;
      for (int i = 0; i < info_len; ++i)
        file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
    }
    file.close();
  }

  //读出第n个int的值赋给tmp，1_base
  void get_info(int &tmp, int n) {
    if (n > info_len) return;
    file.open(file_name, std::fstream::in | std::fstream::binary);
    file.seekg((n - 1) * sizeof(int));
    file.read(reinterpret_cast<char *>(&tmp), sizeof(int));
    file.close();
  }

  //将tmp写入第n个int的位置，1_base
  void write_info(int tmp, int n) {
    if (n > info_len) return;
    file.open(file_name, std::fstream::in | std::fstream::out | std::fstream::binary);
    file.seekp((n - 1) * sizeof(int));
    file.write(reinterpret_cast<char *>(&tmp), sizeof (int));
    file.close();
  }

  //在文件合适位置写入类对象t，并返回写入的位置索引index
  //位置索引意味着当输入正确的位置索引index，在以下三个函数中都能顺利的找到目标对象进行操作
  //位置索引index可以取为对象写入的起始位置
  void write(T &t, const int index) {
    //file.open(file_name, std::fstream::in | std::fstream::out | std::fstream::binary);
    file.seekp(static_cast<long long int>(index) * sizeofT + info_len * sizeof(int));
    file.write(reinterpret_cast<const char *>(&t), sizeofT);
    //file.close();
  }

  //读出位置索引index对应的T对象的值并赋值给t，保证调用的index都是由write函数产生
  void read(T &t, const int index) {
    //file.open(file_name, std::fstream::in | std::fstream::binary);
    file.seekg(static_cast<long long int>(index) * sizeofT + info_len * sizeof(int));
    file.read(reinterpret_cast<char *>(&t), sizeofT);
    //file.close();
  }

  void open() {
    file.open(file_name, std::fstream::in | std::fstream::out | std::fstream::binary);
  }
  void close() {
    file.close();
  }
};

#endif //MEMORYRIVER_HPP