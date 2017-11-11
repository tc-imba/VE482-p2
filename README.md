# LemonDB

## Compilation
```
mkdir build && cd build
cmake .. && make lemondb
```

## Testing
The test cases are too bug, so they are stored with Git LFS. See more information on [Git LFS pages](https://git-lfs.github.com/).

Once Git LFS extension is installed, you can download the test cases through cloning the submodule:
```
git submodule init
git submodule update
```

Set the working directory as `testcase/sample`, set the program argument as `*.query`, simply start debugging!
(The loading query in all test files should be based on `testcase/sample` directory)