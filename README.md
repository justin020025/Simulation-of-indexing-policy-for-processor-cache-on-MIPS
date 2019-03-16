# Simulation-of-indexing-policy-for-processor-cache-on-MIPS
This is a C++ program which simulates the cache behavior on MIPS instruction set architecture and the performance is tested thourgh some data. This is a project assigned by Ting-Ting Hua, professor in department of computer science, NTHU.
## Purpose
Cache indexing policy is one of the concern of CPU performance. Taking the most significant bits (after some other indexing bits) as the indexing bits may be good due to locality, but for embedded system designed for particular use, it can work better if choosing the bits based on the often-used operations. The program assumes <b>LRU</b> policy. The block staying longest in a full cache set will be replaced if another block is coming into the cache set. The indexing policy is based on the paper <I>Zero Cost Indexing for Improved Processor
Cache Performance</I> by TONY GIVARGIS, 2006.
## Usage
Open command window and direct to the program_and_data folder. There are 6 kinds of cache information and 8 testcases, totally 48 combination. Run each of the 48 command in lsb_command.txt and the result (cache miss or hit and miss count) of the cache applying LSB indexing policy will be in index.rpt. Same for the opitimum indexing cache. The binary number shows which bits are taken as the indexing bits.
## Report
See report.docx (in Chinese).
