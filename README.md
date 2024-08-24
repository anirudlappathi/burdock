
<br/>
<div align="center">

<h3 align="center">burdock</h3>
<p align="center">
A completely local vector database made in C++


  


</p>
</div>

## About The Project

![Burdock Image](https://github.com/anirudlappathi/burdock/blob/main/burdock.jpg)

Burdock was created completely using C++. The project uses the file system and a custom filetype (.brdk) to store your vector files. Burdock can query your database using cosine similarity search to find the closest vectors.

Burdock was created to be a free and local alternative to other vector databases that require you to store your data on the cloud. With Burdock being open source, you can trust that your vectors are completely private.
### Built With

The code is built with:

- [C++](https://cplusplus.com/)
- [Catch2](https://github.com/catchorg/Catch2)
- [CMake](https://cmake.org/cmake/help/book/mastering-cmake/chapter/Writing%20CMakeLists%20Files.html)
## Getting Started

This is an example of how you may give instructions on setting up your project locally.
To get a local copy up and running follow these simple example steps.
### Installation

_Below is an example of how you can instruct your audience on installing and setting up your app. This template doesn't rely on any external dependencies or services._

1. Clone the repo
   ```sh
   git clone https://github.com/anirudlappathi/burdock.git
   ```
2. CD into the repository folder
3. Create a build folder
   ```sh
   mkdir build
   ```
3. CD into the build folder
   ```sh
   cd build  
   ```
4. Make the proper cmake files
   ```sh
   cmake ..
   ```
## Usage

Example:
```cpp
    VectorDatabase vdb("./testing", 5);
    for (int i = 0; i < 16; i++) {
        // The embedding function without
        // any vector inserted creates a random
        // test vector for now
        Embedding e(5);
        vdb.insert(e);
    }
    vdb.WriteToFile();

    // Loading this folder automatically reads
    // in the written files
    VectorDatabase vdb2("./testing");
    std::vector<float> a;
    for (int i = 0; i < 5; i++) {
        a.push_back(1);
    }
    Embedding e(a);
    auto b = vdb2.query(e);
    // The first value contains the vector that is
    // closest to the queried vector and the
    // second value shows the cosine similarity
    // score
    std::cout << *b.first << ", " << b.second << std::endl;
```
## Roadmap

- [x] Add README.md
- [x] Create POC
- [x] Implement KDB-Tree
- [ ] Use KD-Tree within nodes
- [ ] Use various techniques for optimization
  - [ ] Random Projection
  - [ ] Product Quantization
  - [ ] Locality-sensitive hashing
  - [ ] Hierarchical Navigable Small World (HNSW)
- [ ] Python connector library
- [ ] Optimize saving already open databases
  - [ ] When nodes reach max vector limit, increase KDB-Tree by only that single node rather than the entire layer
  - [ ] Save only changed parts of nodes files
- [ ] Monitering
- [ ] Logging
## Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".
Don't forget to give the project a star! Thanks again!

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request
## License

Distributed under the MIT License. See [MIT License](https://opensource.org/licenses/MIT) for more information.
## Contact

anirud.lappathi@gmail.com

Project Link: [https://github.com/anirudlappathi/burdock](https://github.com/anirudlappathi/burdock)
## Acknowledgments

Use this space to list resources you find helpful and would like to give credit to. I've included a few of my favorites to kick things off!


- [anirudlappathi.com](https://anirudlappathi.com)
- [makeread.me](https://github.com/ShaanCoding/ReadME-Generator)
