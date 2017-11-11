# LemonDB

## Compilation
```
mkdir build && cd build
cmake .. && make lemondb
```

## Testing
For a small test case, just use the files under `test` folder. Set the working directory as `test`, set the program argument as `test.query` or `test*.in`.

The test cases are too bug, so they are stored with Git LFS. See more information on [Git LFS pages](https://git-lfs.github.com/).

Once Git LFS extension is installed, you can download the test cases through cloning the submodule:
```
git submodule init
git submodule update
```

Set the working directory as `testcase/sample`, set the program argument as `*.query`, simply start debugging!
(The loading query in all test files should be based on `testcase/sample` directory)