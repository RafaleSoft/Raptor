#include "GLBuilder.h"
#include <GL/gl.h>
#include <iostream>

int main(int argc, char *argv[])
{
  CGLBuilder builder;

  const vector<CGLBuilder::EXTENSION>& extensions = builder.getExtensions();
  for (unsigned int i=0;i<extensions.size();i++)
	{
		const CGLBuilder::EXTENSION& extension = extensions[i];
    std::cout << "extension: " << extension.extensionName << std::endl;
  }

  return 0;
}
