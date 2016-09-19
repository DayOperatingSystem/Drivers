#include <string.h>
#include "RAMFilesystem.h"
#include <stdlib.h>

bool RAMFilesystem::handle(message_t& msg)
{
	if(IO::FileSystem::handle(msg))
		return true;

	return false;
}

file_handle_t RAMFilesystem::open(const char* path, VFS_OPEN_MODES mode)
{
	auto fileIter = files.find(path);
	if(fileIter == files.end() && mode & VFS_MODE_WO)
	{
		currentId++;
		
		std::string strPath = path;
		File* file = new File();
		
		file->name = strPath.substr(strPath.find_last_of('/') + 1);
		file->size = 0;
		file->id = currentId;
		
		files[path] = file;
		handleFiles[currentId] = file;
		
		if(!addToParent(path, file))
			debug_printf("FIXME: Could not add file to parent!\n");
		
		return currentId;
	}
	else if(fileIter == files.end())
		return 0;

	return fileIter->second->id;
}

void RAMFilesystem::close(file_handle_t handle)
{
	DSTUB;
}

size_t RAMFilesystem::read(file_handle_t fhandle, pid_t receiver, size_t offset, size_t size)
{
	auto file = handleFiles.find(fhandle);
	File* f;
	
	if(file == handleFiles.end() || (f = (File*) file->second) && offset > f->size)
		return 0;

	size_t actualSize = 0;

	actualSize = std::min(offset + size, f->size - offset);
	write_message_stream(f->content + offset, actualSize, receiver);
	//memcpy(data, f->content + offset, size);
	return actualSize;
}

size_t RAMFilesystem::write(file_handle_t fhandle, void* data, size_t offset, size_t size)
{
	auto file = handleFiles.find(fhandle);
	File* f;
	
	if(file == handleFiles.end() || !(f = (File*) file->second) || offset > f->size)
		return 0;

	// Append to end
	if(f->size == offset)
	{
		f->size += size;
		f->content = (char*) realloc(f->content, f->size);
		memcpy(f->content + offset, data, size);

		return size;
	}

	// Insert in the middle
	f->size += size;
	f->content = (char*) realloc(f->content, f->size);

	// Move the memory away so it's not overwritten
	memmove(f->content + offset, f->content + offset + size, size);

	// Copy buffer into file data
	memcpy(f->content + offset, data, size);
	return size;
}

bool RAMFilesystem::remove(file_handle_t file)
{
	DSTUB;
}

bool RAMFilesystem::move(file_handle_t dir, const char* path)
{
	DSTUB;
}

file_handle_t RAMFilesystem::opendir(const char* path, ino_t* id, ino_t* nid)
{
	auto iter = files.find(path);
	if(iter == files.end())
		return 0;
	
	auto dir = static_cast<Directory*>(iter->second);
	if(dir->children.size() == 0)
		return -1;
	
	*nid = dir->children.begin()->second->id;
	*id = dir->id;
	
	return *nid;
}

file_handle_t RAMFilesystem::readdir(file_handle_t dir, size_t idx, struct vfs_file* dest)
{
	if(idx == -1)
		return 0;
	
	auto directory = (Directory*) handleFiles[dir];
	if(directory == nullptr)
		return 0;

	auto child = directory->children.find(idx);
	if(child == directory->children.end())
		return 0;
	
	strncpy(dest->path, child->second->name.c_str(), sizeof(dest->path));
	child++;
	
	if(child != directory->children.end())
	{
		dest->child_nid = child->second->id;
	}
	
	return dest->nid;
}

bool RAMFilesystem::removeDirectory(file_handle_t file)
{
	DSTUB;
}

file_handle_t RAMFilesystem::createDirectory(const char* path, VFS_OPEN_MODES mode)
{
	auto fileIter = files.find(path);
	if(fileIter == files.end())
	{
		currentId++;
		
		std::string strPath = path;
		Directory* dir = new Directory();
		
		dir->name = strPath.substr(strPath.find_last_of('/') + 1);
		dir->id = currentId;

		files[path] = dir;
		handleFiles[currentId] = dir;
		
		if(strcmp(path, "/") && !addToParent(path, dir))
			debug_printf("FIXME: Could not add directory to parent!\n");
		
		return currentId;
	}
	
	return 0;
}

void RAMFilesystem::changeOwner(file_handle_t node, uid_t user)
{
	DSTUB;
}

void RAMFilesystem::changeMode(file_handle_t node, mode_t mode)
{
	DSTUB;
}

bool RAMFilesystem::fstat(file_handle_t handle, struct stat* st)
{
	auto node = handleFiles.find(handle);
	if(node == handleFiles.end())
		return false;

	File* file = (File*) node->second;
	
	// TODO: Fill in the rest!
	st->st_size = file->size;
	st->st_uid = file->uid;
	st->st_gid = file->gid;
	st->st_ino = handle;

	return true;
}


bool RAMFilesystem::addToParent(const char* fullpath, Node* node)
{
	std::string strpath = fullpath;
	int idx = strpath.find_last_of("/");
	if(idx == -1)
		return false;
	
	if(idx == 0)
	{
		static_cast<Directory*>(files["/"])->children[node->id] = node;
		return true;
	}
	
	strpath.erase(idx);
	auto iter = files.find(strpath);
	if(iter == files.end())
	{
		return false;
	}
	
	((Directory*) iter->second)->children[node->id] = node;
	return true;
}
