#include "config.h"

class Material
{
public:
	Material(const char* filename);
	~Material();
	void use(int unit) const;

private:
	unsigned int texture;
};