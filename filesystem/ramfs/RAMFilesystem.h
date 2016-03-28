#ifndef RAMFILESYSTEM_H
#define RAMFILESYSTEM_H

#include <FileSystem.h>
#include <map>
#include <vector>
#include <sys/stat.h>
#include <memory>
#include "BinaryTree.h"

enum NODE_TYPE
{
	GENERAL_NODE = 0,
	FILE_NODE,
	DIRECTORY_NODE
};

class FSNode
{
public:
	FSNode* parent;
	unsigned int id;
	std::string name;
	struct stat info;

	virtual NODE_TYPE getType() = 0;
};

class File : public FSNode
{
public:
	~File() { delete content; }
	char* content = NULL;
	size_t size = 0;
	uid_t uid = 0;
	gid_t gid = 0;

	virtual NODE_TYPE getType() { return FILE_NODE; }

	void alloc(size_t sz, size_t offset);
	void write(void* data, size_t sz, size_t offset);
	size_t read(void* data, size_t sz, size_t offset);
};

class Directory : public FSNode
{
private:
	std::vector<std::shared_ptr<FSNode>> m_children;
public:

	virtual NODE_TYPE getType() { return DIRECTORY_NODE; }

	void addChild(std::shared_ptr<FSNode> node) { m_children.push_back(node); }
	std::shared_ptr<FSNode> getChild(size_t idx) { return m_children[idx]; }
	size_t getNumChildren() { return m_children.size(); }
};

class RAMFilesystem : public IO::FileSystem
{
	std::map<std::string, std::shared_ptr<FSNode>> m_nodes;
	BinaryTree<std::shared_ptr<FSNode>> m_files;
	long unsigned int m_id;

	std::shared_ptr<FSNode> m_root;

public:
	RAMFilesystem() : IO::FileSystem()
	, m_id(1)
	{
		m_nodes["/"] = *m_files.insert(m_root = std::shared_ptr<FSNode>(new Directory), 0);
	}

	bool initialize();
	bool handle(message_t& msg);
	const char* getName() { return "RAMFS"; }

	virtual file_handle_t open(const char* path, VFS_OPEN_MODES mode);
	virtual void close(file_handle_t handle);

	virtual size_t read(file_handle_t, void* data, size_t offset, size_t size);
	virtual size_t write(file_handle_t file, void* data, size_t offset, size_t size);
	virtual bool remove(file_handle_t file);
	virtual bool move(file_handle_t dir, const char* path);
	virtual bool fstat(file_handle_t handle, struct stat* st);

	virtual file_handle_t opendir(const char* path);
	virtual file_handle_t readdir(file_handle_t dir, size_t idx);
	virtual bool removeDirectory(file_handle_t file);
	virtual file_handle_t createDirectory(const char* path);

	virtual void changeOwner(file_handle_t node, uid_t user);
	virtual void changeMode(file_handle_t node, mode_t mode);	
};

#endif // RAMFILESYSTEM_H
