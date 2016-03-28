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
	auto iter = m_nodes.find(path);

	// If it does not exist, create the file if it is set to writing
	if(iter == m_nodes.end() && mode & VFS_MODE_WO)
	{
		std::string parentpath = path;
		int idx = parentpath.find_last_of("/");

		// If the path is invalid, return.
		if(idx == -1)
			return -1;

		// Save the file name
		std::string name = parentpath.substr(idx + 1);

		// Remove actual filename
		parentpath.erase(idx);

		// If we found root
		if(parentpath.empty())
			parentpath = "/";

		/// TODO: Check rights!
		Directory* parent = dynamic_cast<Directory*>(m_nodes[parentpath].get());
		if(!parent)
			return -1;

		std::shared_ptr<FSNode> fnod = *m_files.insert(std::make_shared<File>(), m_id);

		File* f = static_cast<File*>(fnod.get());
		f->name = name;
		f->id = m_id;
		f->parent = parent;

		parent->addChild(fnod);

		// Register in file table
		m_nodes[path] = fnod;
		m_id++;

		return f->id;
	}
	else if(iter == m_nodes.end())
	{
		// If it is not set to write, return as the file could not be read.
		return 0;
	}

	return iter->second->id;
}

void RAMFilesystem::close(file_handle_t handle)
{

}

size_t RAMFilesystem::read(file_handle_t fhandle, void* data, size_t offset, size_t size)
{
	std::shared_ptr<FSNode> pf = m_files.find(fhandle);
	File* f;

	if(pf == nullptr || !(f = dynamic_cast<File*>(pf.get())) || offset > f->size)
		return 0;

	size_t actualSize = 0;

	actualSize = std::min(offset + size, f->size - offset);
	//write_message_stream(f->content + rq->offset, size, sender);
	memcpy(data, f->content + offset, size);
	return actualSize;
}

size_t RAMFilesystem::write(file_handle_t fhandle, void* data, size_t offset, size_t size)
{
	std::shared_ptr<FSNode> pf = m_files.find(fhandle);
	File* f;

	if(pf == nullptr || !(f = dynamic_cast<File*>(pf.get())) || offset > f->size)
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
	// Will go out of scope and delete the memory associated
	std::shared_ptr<FSNode> node = m_files.find(file);
	if(node == nullptr)
		return false;

	auto iter = m_nodes.begin();
	while(iter != m_nodes.end())
		if(iter->second->id == file)
		{
			m_nodes.erase(iter);
			break;
		}
		else iter++;

	// Remove from binary tree
	m_files.remove(file);
	return true;
}

bool RAMFilesystem::move(file_handle_t dir, const char* path)
{
	
}

file_handle_t RAMFilesystem::opendir(const char* path)
{
	
}

file_handle_t RAMFilesystem::readdir(file_handle_t dir, size_t idx)
{
	
}

bool RAMFilesystem::removeDirectory(file_handle_t file)
{
	
}

file_handle_t RAMFilesystem::createDirectory(const char* path)
{
	
}

void RAMFilesystem::changeOwner(file_handle_t node, uid_t user)
{
	
}

void RAMFilesystem::changeMode(file_handle_t node, mode_t mode)
{
	
}

bool RAMFilesystem::fstat(file_handle_t handle, struct stat* st)
{
	std::shared_ptr<FSNode> node = m_files.find(handle);
	File* file;

	if(node == nullptr || !(file = dynamic_cast<File*>(node.get())))
		return false;

	// TODO: Fill in the rest!
	st->st_size = file->size;
	st->st_uid = file->uid;
	st->st_gid = file->gid;
	st->st_ino = handle;

	return true;
}
