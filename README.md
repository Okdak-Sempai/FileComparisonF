# FileComparison
An optimised tool to compare 2 files, mostly useful for Corruption test.

# PURPOSE

This tool's purpose is to compare 2 files and determine if one of them is different and which one differs.

This should be used when you have twice the same file but both have the same size. How to know if they are exact copies or not? Use this tool.

# FEATURES
There is 2 main methods.
- compareFiles() that compares literally byte per byte. This is slower one but technically the most accurate one.
- compareFilesCluster() that uses Clusters of 4096 bytes. This is the faster one but technically less accurate
