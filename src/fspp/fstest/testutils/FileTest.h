#pragma once
#ifndef MESSMER_FSPP_FSTEST_TESTUTILS_FILETEST_H_
#define MESSMER_FSPP_FSTEST_TESTUTILS_FILETEST_H_

#include "FileSystemTest.h"
#include <cpp-utils/data/Data.h>
#include <cpp-utils/pointer/unique_ref.h>
#include <cpp-utils/system/stat.h>

template<class ConcreteFileSystemTestFixture>
class FileTest: public FileSystemTest<ConcreteFileSystemTestFixture> {
public:
  FileTest(): file_root(), file_nested() {
	this->LoadDir("/")->createAndOpenFile("myfile", this->MODE_PUBLIC, 0, 0);
	file_root = this->LoadFile("/myfile");
	file_root_node = this->Load("/myfile");

	this->LoadDir("/")->createDir("mydir", this->MODE_PUBLIC, 0, 0);
	this->LoadDir("/mydir")->createAndOpenFile("mynestedfile", this->MODE_PUBLIC, 0, 0);
	file_nested = this->LoadFile("/mydir/mynestedfile");
	file_nested_node = this->Load("/mydir/mynestedfile");

	this->LoadDir("/")->createDir("mydir2", this->MODE_PUBLIC, 0, 0);
  }
  std::unique_ptr<fspp::File> file_root;
  std::unique_ptr<fspp::File> file_nested;
  std::unique_ptr<fspp::Node> file_root_node;
  std::unique_ptr<fspp::Node> file_nested_node;

  //TODO IN_STAT still needed after moving it to FsppNodeTest?
  void IN_STAT(fspp::File *file, fspp::Node *node, std::function<void (const fspp::Node::stat_info&)> callback) {
	  auto st1 = node->stat();
	  callback(st1);
	  auto st2 = file->open(O_RDONLY)->stat();
	  callback(st2);
  }

  void EXPECT_SIZE(uint64_t expectedSize, fspp::File *file, fspp::Node *node) {
	IN_STAT(file, node, [expectedSize] (const fspp::Node::stat_info& st) {
		EXPECT_EQ(expectedSize, static_cast<uint64_t>(st.size));
	});

	EXPECT_NUMBYTES_READABLE(expectedSize, file);
  }

  void EXPECT_NUMBYTES_READABLE(uint64_t expectedSize, fspp::File *file) {
	auto openFile = file->open(O_RDONLY);
	cpputils::Data data(expectedSize);
	//Try to read one byte more than the expected size
	ssize_t readBytes = openFile->read(data.data(), expectedSize+1, 0);
	//and check that it only read the expected size (but also not less)
	EXPECT_EQ(expectedSize, static_cast<uint64_t>(readBytes));
  }

  void EXPECT_ATIME_EQ(struct timespec expected, const fspp::Node::stat_info& st) {
	  EXPECT_EQ(expected.tv_sec, st.atime.tv_sec);
	  EXPECT_EQ(expected.tv_nsec, st.atime.tv_nsec);
  }

  void EXPECT_MTIME_EQ(struct timespec expected, const fspp::Node::stat_info& st) {
      EXPECT_EQ(expected.tv_sec, st.mtime.tv_sec);
      EXPECT_EQ(expected.tv_nsec, st.mtime.tv_nsec);
  }
};

#endif
