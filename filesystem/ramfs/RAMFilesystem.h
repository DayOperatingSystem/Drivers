#ifndef RAMFILESYSTEM_H
#define RAMFILESYSTEM_H

#include <FileSystem.h>
#include <map>
#include <vector>
#include <string>
#include <sys/stat.h>
#include <memory>

struct Node
{
	std::string name;
	DEVICE_TYPES type;
	file_handle_t id;
	
	uid_t uid = 0;
	gid_t gid = 0;
};

struct File : public Node
{
	File() { type = VFS_REGULAR; content = NULL; size = 0; }
	char* content;
	size_t size;
};

struct Directory : public Node
{	
	Directory() { type = VFS_DIRECTORY; }
	std::map<file_handle_t, Node*> children;
};

class RAMFilesystem : public IO::FileSystem
{
	std::map<std::string, Node*> files;
	std::map<file_handle_t, Node*> handleFiles;
	
	file_handle_t currentId;
	
	bool addToParent(const char* fullpath, Node* node);
public:
	RAMFilesystem() : IO::FileSystem()
	{ currentId = 0; createDirectory("/", VFS_MODE_RW); }

	bool initialize() { currentId = 0; }
	bool handle(message_t& msg);
	const char* getName() { return "RAMFS"; }

	virtual file_handle_t open(const char* path, VFS_OPEN_MODES mode);
	virtual void close(file_handle_t handle);

	virtual size_t read(file_handle_t, pid_t receiver, size_t offset, size_t size);
	virtual size_t write(file_handle_t file, void* data, size_t offset, size_t size);
	virtual bool remove(file_handle_t file);
	virtual bool move(file_handle_t dir, const char* path);
	virtual bool fstat(file_handle_t handle, struct stat* st);

	virtual file_handle_t opendir(const char* path, ino_t* id, ino_t* nid);
	virtual file_handle_t readdir(file_handle_t dir, size_t idx, struct vfs_file* dest);
	virtual bool removeDirectory(file_handle_t file);
	virtual file_handle_t createDirectory(const char* path, VFS_OPEN_MODES mode);

	virtual void changeOwner(file_handle_t node, uid_t user);
	virtual void changeMode(file_handle_t node, mode_t mode);	
};

#endif // RAMFILESYSTEM_H
