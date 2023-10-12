# FileComparisonF
##### An optimised tool to compare 2 files, mostly useful for Corruption test.

This is a derivative of FileComparison that does the same work but compare files in batch.

# PURPOSE

This tool's purpose is to compare 2 folders that contains files with the same name and determine if one of them is different and which one differs.

This should be used when you have twice the same file but both have the same size or even same name(or different). 
How to know if they are exact copies or not? Use this tool.

# FEATURES
There is 2 main methods.
- compareFiles() that compares literally byte per byte. This is slower one but technically the most accurate one.
- compareFilesCluster() that uses Clusters of 4096 bytes. This is the faster one but technically less accurate

A log for the files that were differents( or Identicals) can be generated.

## NOTICE

WARNING: Only use it to compare folders with the same number of files and the same names for files to be compared. (In each folders.)
