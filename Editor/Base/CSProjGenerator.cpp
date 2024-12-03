#include "Project.hpp"
#include "mpi/Core/Guid.hpp"
#include <mpi/Filesystem/File.hpp>
#include <mpi/Filesystem/Filesystem.hpp>

const static char slnFilePattern[] = u8"Microsoft Visual Studio Solution File, Format Version 12.00\n" \
"Project(\"{%s}\") = \"%s\", \"%s\", \"{%s}\"\n" \
"EndProject\n" \
"Global\n" \
"	GlobalSection(SolutionConfigurationPlatforms) = preSolution\n" \
"		Debug|Any CPU = Debug|Any CPU\n" \
"		Release|Any CPU = Release|Any CPU\n" \
"	EndGlobalSection\n" \
"	GlobalSection(ProjectConfigurationPlatforms) = postSolution\n" \
"		{%s}.Debug|Any CPU.ActiveCfg = Debug|Any CPU\n" \
"		{%s}.Debug|Any CPU.Build.0 = Debug|Any CPU\n" \
"		{%s}.Release|Any CPU.ActiveCfg = Release|Any CPU\n" \
"		{%s}.Release|Any CPU.Build.0 = Release|Any CPU\n" \
"	EndGlobalSection\n" \
"	GlobalSection(SolutionProperties) = preSolution\n" \
"		HideSolutionNode = FALSE\n" \
"	EndGlobalSection\n" \
"EndGlobal\n";

const static char csprojFilePattern[] = u8"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n" \
"<Project ToolsVersion=\"12.0\" DefaultTargets=\"Build\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n" \
"  <PropertyGroup>\n" \
"    <Configuration Condition=\" '$(Configuration)' == '' \">Debug</Configuration>\n" \
"    <Platform Condition=\" '$(Platform)' == '' \">AnyCPU</Platform>\n" \
"    <ProductVersion>10.0.20506</ProductVersion>\n" \
"    <SchemaVersion>4.0</SchemaVersion>\n" \
"    <ProjectGuid>{%s}</ProjectGuid>\n" \
"    <OutputType>Library</OutputType>\n" \
"    <AppDesignerFolder>Properties</AppDesignerFolder>\n"
"    <RootNamespace>\n" \
"    </RootNamespace>\n" \
"    <AssemblyName>%s</AssemblyName>\n" \
"    <TargetFrameworkVersion>v4.8</TargetFrameworkVersion>\n" \
"    <FileAlignment>512</FileAlignment>\n" \
"    <TargetFrameworkProfile />\n" \
"  </PropertyGroup>\n" \
"  <PropertyGroup Condition=\" '$(Configuration)|$(Platform)' == 'Debug|AnyCPU' \">\n" \
"    <DebugSymbols>true</DebugSymbols>\n" \
"    <DebugType>full</DebugType>\n" \
"    <Optimize>false</Optimize>\n" \
"    <OutputPath>Temp\\bin\\Debug\\</OutputPath>\n" \
"    <DefineConstants>DEBUG;TRACE;VSGE</DefineConstants>\n" \
"    <ErrorReport>prompt</ErrorReport>\n" \
"    <WarningLevel>4</WarningLevel>\n" \
"    <NoWarn>0169</NoWarn>\n" \
"  </PropertyGroup>\n" \
"  <PropertyGroup Condition=\" '$(Configuration)|$(Platform)' == 'Release|AnyCPU' \">\n" \
"    <DebugType>pdbonly</DebugType>\n" \
"    <Optimize>true</Optimize>\n" \
"    <OutputPath>Temp\\bin\\Release\\</OutputPath>\n" \
"    <DefineConstants>TRACE</DefineConstants>\n" \
"    <ErrorReport>prompt</ErrorReport>\n" \
"    <WarningLevel>4</WarningLevel>\n" \
"    <NoWarn>0169</NoWarn>\n" \
"  </PropertyGroup>\n" \
"  <ItemGroup>\n" \
"    <Reference Include=\"System\" />\n" \
"    <Reference Include=\"System.XML\" />\n" \
"    <Reference Include=\"System.Core\" />\n" \
"    <Reference Include=\"VSGE\">\n" \
"      <HintPath>%s</HintPath>\n" \
"    </Reference>\n" \
"  </ItemGroup>\n" \
"  <ItemGroup Label=\"Scripts\">\n" \
"    <Compile Include=\"..\\assets\\*.cs\" />\n" \
"  </ItemGroup>\n" \
"  <Import Project=\"$(MSBuildToolsPath)\\Microsoft.CSharp.targets\" />\n" \
"</Project>";

size_t makeSlnContents(char* buffer, size_t size, 
	const Guid& slnGuid,
	const std::string& projectName,
	const std::string& csprojFileName,
	const Guid& csprojGuid)
{
	return std::snprintf(buffer, size, slnFilePattern,
		slnGuid.toString().c_str(),
		projectName.c_str(),
		csprojFileName.c_str(),
		csprojGuid.toString().c_str(),
		// params
		csprojGuid.toString().c_str(),
		csprojGuid.toString().c_str(),
		csprojGuid.toString().c_str(),
		csprojGuid.toString().c_str());
}

size_t makeCsprojContents(char* buffer, size_t size,
	const Guid& csprojGuid,
	const std::string& projectName,
	const std::string& dllPath)
{
	return std::snprintf(buffer, size, csprojFilePattern,
		csprojGuid.toString().c_str(),
		projectName.c_str(),
		dllPath.c_str());
}

const std::string CreateProjectFiles(const VSGEditor::Project& proj) 
{
	Mpi::File outputDir(proj.GetCsprojDirectory());

	if (!outputDir.isDirectory()) {
		outputDir.mkdir();
	}

	std::string projectName = proj.GetName();
	const std::string slnFilePath = proj.GetCsprojDirectory() + "/" + projectName + ".sln";

	const std::string csprojFileName = projectName + ".csproj";
	const std::string csprojFilePath = proj.GetCsprojDirectory() + "/" + csprojFileName;

	Guid slnGuid;
	Guid csprojGuid;

	size_t slnSize = makeSlnContents(nullptr, 0, slnGuid, projectName, csprojFileName, csprojGuid);
	char* slnContents = new char[slnSize + 1];
	makeSlnContents(slnContents, slnSize + 1, slnGuid, projectName, csprojFileName, csprojGuid);
	std::ofstream slnStream(slnFilePath, std::iostream::binary);
	slnStream.write(slnContents, slnSize);
	slnStream.close();
	delete[] slnContents;

	std::string dllPath;
	Mpi::Filesystem::getAbsPath("api.dll", dllPath);

	size_t csprojSize = makeCsprojContents(nullptr, 0, csprojGuid, projectName, dllPath);
	char* csprojContents = new char[csprojSize + 1];
	makeCsprojContents(csprojContents, csprojSize + 1, csprojGuid, projectName, dllPath);
	std::ofstream csprojStream(csprojFilePath, std::iostream::binary);
	csprojStream.write(csprojContents, csprojSize);
	csprojStream.close();
	delete[] csprojContents;

	return slnFilePath;
}