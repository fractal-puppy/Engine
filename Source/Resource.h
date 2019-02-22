#ifndef __Resource_h__
#define __Resource_h__

#include <string>

struct JSON_value;

enum class TYPE
{
	TEXTURE,
	MESH,
	AUDIO,
	SCENE,
	BONE,
	ANIMATION,
	UNKNOWN
};

/*	A Resource is any file with a known format dropped in /Assets/
	A copy of this will be automaticaly imported to /Library/ with 
	the engine own format and a .meta file saved next to the /Assets/ file.

	WARNING:
	- Do not delete the .meta file.
	- Do not delete the files saved in Library.
	- Deleting, moving or renaming of any file located on /Assets/ from 
	external file browsers is highly discouraged. Always use the engine
	to perform this actions.
*/

class Resource
{
public:
	Resource(unsigned uid, TYPE type);
	virtual ~Resource();

	inline TYPE			GetType() const	{return type;};
	inline unsigned		GetUID() const	{ return UID; };
	inline const char*	GetFile() const	{ return file.c_str(); };
	inline void			SetFile(const char* newFile) { file = newFile; };
	inline const char*	GetExportedFile() const {return exportedFile.c_str();};
	inline void			SetExportedFile(const char* newExportedFile) { exportedFile = newExportedFile; };
	inline bool			IsLoadedToMemory() const { return loaded > 0; };
	inline unsigned		GetReferences() const { return loaded; };
	inline void			SetReferences(unsigned references) { loaded = references; };
	
	virtual void Save(JSON_value &config) const;
	virtual void Load(const JSON_value &config);
	virtual bool LoadInMemory() = 0;
	virtual void DeleteFromMemory() { loaded = 0; };

protected:
	unsigned UID = 0u;
	std::string file;			// The file located in /Assets/
	std::string exportedFile;	// The file located in /Library/
	unsigned loaded = 0;		// Number of times this resource is being used in memory

	TYPE type = TYPE::UNKNOWN;	// Type of resource
};

#endif __Resource_h__