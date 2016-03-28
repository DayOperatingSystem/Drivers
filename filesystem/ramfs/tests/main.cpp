#include <tests.h>
#include "../BinaryTree.h"
#include "../RAMFilesystem.h"

TEST(insert, Tree, 0.0f,
// initialisation
	 {

	 },
// cleanup
	 {
		m_data.tree.clear();
	 },
// test
	 {
		// Adding stuff
		 m_data.tree.insert(5, 5);
		 m_data.tree.insert(6, 6);
		 m_data.tree.insert(7, 7);

		 // Checking find
		 ASSERT(m_data.tree.find(5) == 5);
		 ASSERT(m_data.tree.find(6) == 6);
		 ASSERT(m_data.tree.find(7) == 7);
	 },
// data
	 {
		BinaryTree<int> tree;
	 }
)

TEST(remove, Tree, 0.0f,
// initialisation
	 {

	 },
// cleanup
	 {
		 m_data.tree.clear();
	 },
// test
	 {
		 // Adding stuff
		 m_data.tree.insert(5, 5);
		 m_data.tree.insert(6, 6);
		 m_data.tree.insert(7, 7);

		 m_data.tree.remove(7);

		 // Checking find
		 ASSERT(m_data.tree.find(5) == 5);
		 ASSERT(m_data.tree.find(6) == 6);
		 ASSERT(m_data.tree.find(7) == 0); // It just returns an empty element if nothing can be found
	 },
// data
	 {
		 BinaryTree<int> tree;
	 }
)

TEST(open, RAMFS, 0.0f,
// initialisation
	 {
		m_data.file = 0;
	 },
// cleanup
	 {
		 m_data.rfs.remove(m_data.file);
	 },
// test
	 {
		 m_data.file = m_data.rfs.open("/test.txt", VFS_MODE_RW);
		 ASSERT(m_data.file != 0);
	 },
// data
	 {
		 file_handle_t file;
		 RAMFilesystem rfs;
	 }
)

TEST(remove, RAMFS, 0.0f,
// initialisation
	 {
		 m_data.file = m_data.rfs.open("/test.txt", VFS_MODE_RW);
	 },
// cleanup
	 {
		m_data.file = 0;
	 },
// test
	 {
		 m_data.rfs.remove(m_data.file);
		 ASSERT(m_data.rfs.open("/test.txt", VFS_MODE_RO) == 0);
	 },
// data
	 {
		 file_handle_t file;
		 RAMFilesystem rfs;
	 }
)