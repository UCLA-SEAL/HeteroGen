# HeteroGen
HeteroGen: transpiling C to heterogeneous HLS code with automated test generation and program repair (ASPLOS 2022)

## Summary 
Despite the trend of incorporating heterogeneity and specialization in hardware, the development of heterogeneous applications is limited to a handful of engineers with deep hardware expertise. We propose HeteroGen that takes C/C++ code as input and automatically generates an HLS version with test behavior preservation and better performance. Key to the success of HeteroGen is adapting the idea of search-based program repair to the heterogeneous computing domain, while addressing two technical challenges. First, the turn-around time of HLS compilation and simulation is much longer than the usual C/C++ compilation and execution time; therefore, HeteroGen applies pattern-oriented program edits guided by common fix patterns and their dependences. Second, behavior and performance checking requires testing, but test cases are often unavailable. Thus, HeteroGen auto-generates test inputs suitable for checking C to HLS-C conversion errors, while providing high branch coverage for the original C code.
An evaluation of HeteroGen shows that it produces an HLS-compatible version for nine out of ten real-world heterogeneous applications fully automatically, applying up to 438 lines of edits to produce an HLS version 1.63x faster than the original version.

## Team 
This project is led by Professor Miryung Kim(http://web.cs.ucla.edu/~miryung)'s group at UCLA. 

[Qian Zhang](http://web.cs.ucla.edu/~zhangqian/): Postdoctoral researcher, zhangqian@cs.ucla.edu;

[Jiyuan Wang](http://web.cs.ucla.edu/~wangjiyuan): PhD student, wangjiyuan@cs.ucla.edu;

[Harry Xu](http://web.cs.ucla.edu/~harryxu/): Associate Professor, harryxu@cs.ucla.edu; 

[Miryung Kim](http://web.cs.ucla.edu/~miryung/): Professor at UCLA, miryung@cs.ucla.edu;


## How to cite 
Please refer to our ASPLOS'22 paper, [HeteroGen: transpiling C to heterogeneous HLS code with automated test generation and program repair](http://web.cs.ucla.edu/~miryung/Publications/ASPLOS22_heterogen.pdf) for more details. 
### Bibtex  
@inproceedings{10.1145/3503222.3507748, author = {Zhang, Qian and Wang, Jiyuan and Xu, Guoqing Harry and Kim, Miryung}, title = {HeteroGen: Transpiling C to Heterogeneous HLS Code with Automated Test Generation and Program Repair}, year = {2022}, isbn = {9781450392051}, publisher = {Association for Computing Machinery}, address = {New York, NY, USA}, url = {https://doi.org/10.1145/3503222.3507748}, doi = {10.1145/3503222.3507748}, booktitle = {Proceedings of the 27th ACM International Conference on Architectural Support for Programming Languages and Operating Systems}, pages = {1017–1029}, numpages = {13}, keywords = {test generation, Heterogeneous applications, program repair}, location = {Lausanne, Switzerland}, series = {ASPLOS 2022} }
[DOI Link](https://dl.acm.org/doi/10.1145/3503222.3507748)


# HeteroGen (asplos2022-artifacts)
Artifact submission for ASPLOS 2022.

## Artifacts
As described in our extended abstract, the artifacts of HeteroGen include:
- **HLS error study**: 1-error-study.pdf
- **Fuzzing-based test generation**: `2-test-generation` folder
- **Auomated code edits for error removal**: `3-repair` folder
- `test-program` contains our running example for this artifact submission.

Please follow the instructions in each folder.

