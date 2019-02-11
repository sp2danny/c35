##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=c35
ConfigurationName      :=Debug
WorkspacePath          := "/home/sp2danny/extra/c35"
ProjectPath            := "/home/sp2danny/extra/c35"
IntermediateDirectory  :=./bin/Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Daniel Nyström
Date                   :=02/11/19
CodeLitePath           :="/home/sp2danny/.codelite"
LinkerName             :=/usr/bin/clang++ 
SharedObjectLinkerName :=/usr/bin/clang++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(ProjectName)_dbg.out
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="c35.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch)src $(IncludeSwitch)inc $(IncludeSwitch)/usr/include/SDL $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)SDL 
ArLibs                 :=  "SDL" 
LibPath                := $(LibraryPathSwitch)/usr/lib/x86_64-linux-gnu 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := /usr/bin/ar rcu
CXX      := /usr/bin/clang++ 
CC       := /usr/bin/clang 
CXXFLAGS := -Wall -Wextra -Werror -pedantic -std=c++17 -Wno-gnu-anonymous-struct -Wno-nested-anon-types -Wno-c++11-narrowing -Wno-unused-function -Wno-unused-variable -Wno-missing-field-initializers -Wno-unused-parameter -Wno-sign-compare -g -O0 $(Preprocessors)
CFLAGS   := -Wall -g -O0 $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as 


##
## User defined environment variables
##
Objects0=$(IntermediateDirectory)/src_Main.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_FrameSystem.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_AI.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Anim.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_AssetRepository.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Board.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_City.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_CityView.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Font.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_GUI.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/src_GameEngine.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Hexagon.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_LoadScreen.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_LockedAnimation.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_MakeFrame.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Menu.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_MiniMap.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Player.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Resource.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_TSD.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/src_Tech.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Unit.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_UnitAction.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_pcxLoad.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_resize++.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_sdlGuiColl.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_stdafx.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

$(IntermediateDirectory)/.d:
	@test -d ./bin/Debug || $(MakeDirCommand) ./bin/Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/src_Main.cpp$(ObjectSuffix): src/Main.cpp $(IntermediateDirectory)/src_Main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sp2danny/extra/c35/src/Main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Main.cpp$(DependSuffix): src/Main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Main.cpp$(DependSuffix) -MM "src/Main.cpp"

$(IntermediateDirectory)/src_Main.cpp$(PreprocessSuffix): src/Main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Main.cpp$(PreprocessSuffix) "src/Main.cpp"

$(IntermediateDirectory)/src_FrameSystem.cpp$(ObjectSuffix): src/FrameSystem.cpp $(IntermediateDirectory)/src_FrameSystem.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sp2danny/extra/c35/src/FrameSystem.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_FrameSystem.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_FrameSystem.cpp$(DependSuffix): src/FrameSystem.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_FrameSystem.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_FrameSystem.cpp$(DependSuffix) -MM "src/FrameSystem.cpp"

$(IntermediateDirectory)/src_FrameSystem.cpp$(PreprocessSuffix): src/FrameSystem.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_FrameSystem.cpp$(PreprocessSuffix) "src/FrameSystem.cpp"

$(IntermediateDirectory)/src_AI.cpp$(ObjectSuffix): src/AI.cpp $(IntermediateDirectory)/src_AI.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sp2danny/extra/c35/src/AI.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_AI.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_AI.cpp$(DependSuffix): src/AI.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_AI.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_AI.cpp$(DependSuffix) -MM "src/AI.cpp"

$(IntermediateDirectory)/src_AI.cpp$(PreprocessSuffix): src/AI.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_AI.cpp$(PreprocessSuffix) "src/AI.cpp"

$(IntermediateDirectory)/src_Anim.cpp$(ObjectSuffix): src/Anim.cpp $(IntermediateDirectory)/src_Anim.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sp2danny/extra/c35/src/Anim.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Anim.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Anim.cpp$(DependSuffix): src/Anim.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Anim.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Anim.cpp$(DependSuffix) -MM "src/Anim.cpp"

$(IntermediateDirectory)/src_Anim.cpp$(PreprocessSuffix): src/Anim.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Anim.cpp$(PreprocessSuffix) "src/Anim.cpp"

$(IntermediateDirectory)/src_AssetRepository.cpp$(ObjectSuffix): src/AssetRepository.cpp $(IntermediateDirectory)/src_AssetRepository.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sp2danny/extra/c35/src/AssetRepository.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_AssetRepository.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_AssetRepository.cpp$(DependSuffix): src/AssetRepository.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_AssetRepository.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_AssetRepository.cpp$(DependSuffix) -MM "src/AssetRepository.cpp"

$(IntermediateDirectory)/src_AssetRepository.cpp$(PreprocessSuffix): src/AssetRepository.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_AssetRepository.cpp$(PreprocessSuffix) "src/AssetRepository.cpp"

$(IntermediateDirectory)/src_Board.cpp$(ObjectSuffix): src/Board.cpp $(IntermediateDirectory)/src_Board.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sp2danny/extra/c35/src/Board.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Board.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Board.cpp$(DependSuffix): src/Board.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Board.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Board.cpp$(DependSuffix) -MM "src/Board.cpp"

$(IntermediateDirectory)/src_Board.cpp$(PreprocessSuffix): src/Board.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Board.cpp$(PreprocessSuffix) "src/Board.cpp"

$(IntermediateDirectory)/src_City.cpp$(ObjectSuffix): src/City.cpp $(IntermediateDirectory)/src_City.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sp2danny/extra/c35/src/City.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_City.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_City.cpp$(DependSuffix): src/City.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_City.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_City.cpp$(DependSuffix) -MM "src/City.cpp"

$(IntermediateDirectory)/src_City.cpp$(PreprocessSuffix): src/City.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_City.cpp$(PreprocessSuffix) "src/City.cpp"

$(IntermediateDirectory)/src_CityView.cpp$(ObjectSuffix): src/CityView.cpp $(IntermediateDirectory)/src_CityView.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sp2danny/extra/c35/src/CityView.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_CityView.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_CityView.cpp$(DependSuffix): src/CityView.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_CityView.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_CityView.cpp$(DependSuffix) -MM "src/CityView.cpp"

$(IntermediateDirectory)/src_CityView.cpp$(PreprocessSuffix): src/CityView.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_CityView.cpp$(PreprocessSuffix) "src/CityView.cpp"

$(IntermediateDirectory)/src_Font.cpp$(ObjectSuffix): src/Font.cpp $(IntermediateDirectory)/src_Font.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sp2danny/extra/c35/src/Font.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Font.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Font.cpp$(DependSuffix): src/Font.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Font.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Font.cpp$(DependSuffix) -MM "src/Font.cpp"

$(IntermediateDirectory)/src_Font.cpp$(PreprocessSuffix): src/Font.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Font.cpp$(PreprocessSuffix) "src/Font.cpp"

$(IntermediateDirectory)/src_GUI.cpp$(ObjectSuffix): src/GUI.cpp $(IntermediateDirectory)/src_GUI.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sp2danny/extra/c35/src/GUI.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_GUI.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_GUI.cpp$(DependSuffix): src/GUI.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_GUI.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_GUI.cpp$(DependSuffix) -MM "src/GUI.cpp"

$(IntermediateDirectory)/src_GUI.cpp$(PreprocessSuffix): src/GUI.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_GUI.cpp$(PreprocessSuffix) "src/GUI.cpp"

$(IntermediateDirectory)/src_GameEngine.cpp$(ObjectSuffix): src/GameEngine.cpp $(IntermediateDirectory)/src_GameEngine.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sp2danny/extra/c35/src/GameEngine.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_GameEngine.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_GameEngine.cpp$(DependSuffix): src/GameEngine.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_GameEngine.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_GameEngine.cpp$(DependSuffix) -MM "src/GameEngine.cpp"

$(IntermediateDirectory)/src_GameEngine.cpp$(PreprocessSuffix): src/GameEngine.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_GameEngine.cpp$(PreprocessSuffix) "src/GameEngine.cpp"

$(IntermediateDirectory)/src_Hexagon.cpp$(ObjectSuffix): src/Hexagon.cpp $(IntermediateDirectory)/src_Hexagon.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sp2danny/extra/c35/src/Hexagon.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Hexagon.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Hexagon.cpp$(DependSuffix): src/Hexagon.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Hexagon.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Hexagon.cpp$(DependSuffix) -MM "src/Hexagon.cpp"

$(IntermediateDirectory)/src_Hexagon.cpp$(PreprocessSuffix): src/Hexagon.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Hexagon.cpp$(PreprocessSuffix) "src/Hexagon.cpp"

$(IntermediateDirectory)/src_LoadScreen.cpp$(ObjectSuffix): src/LoadScreen.cpp $(IntermediateDirectory)/src_LoadScreen.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sp2danny/extra/c35/src/LoadScreen.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_LoadScreen.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_LoadScreen.cpp$(DependSuffix): src/LoadScreen.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_LoadScreen.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_LoadScreen.cpp$(DependSuffix) -MM "src/LoadScreen.cpp"

$(IntermediateDirectory)/src_LoadScreen.cpp$(PreprocessSuffix): src/LoadScreen.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_LoadScreen.cpp$(PreprocessSuffix) "src/LoadScreen.cpp"

$(IntermediateDirectory)/src_LockedAnimation.cpp$(ObjectSuffix): src/LockedAnimation.cpp $(IntermediateDirectory)/src_LockedAnimation.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sp2danny/extra/c35/src/LockedAnimation.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_LockedAnimation.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_LockedAnimation.cpp$(DependSuffix): src/LockedAnimation.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_LockedAnimation.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_LockedAnimation.cpp$(DependSuffix) -MM "src/LockedAnimation.cpp"

$(IntermediateDirectory)/src_LockedAnimation.cpp$(PreprocessSuffix): src/LockedAnimation.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_LockedAnimation.cpp$(PreprocessSuffix) "src/LockedAnimation.cpp"

$(IntermediateDirectory)/src_MakeFrame.cpp$(ObjectSuffix): src/MakeFrame.cpp $(IntermediateDirectory)/src_MakeFrame.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sp2danny/extra/c35/src/MakeFrame.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_MakeFrame.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_MakeFrame.cpp$(DependSuffix): src/MakeFrame.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_MakeFrame.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_MakeFrame.cpp$(DependSuffix) -MM "src/MakeFrame.cpp"

$(IntermediateDirectory)/src_MakeFrame.cpp$(PreprocessSuffix): src/MakeFrame.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_MakeFrame.cpp$(PreprocessSuffix) "src/MakeFrame.cpp"

$(IntermediateDirectory)/src_Menu.cpp$(ObjectSuffix): src/Menu.cpp $(IntermediateDirectory)/src_Menu.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sp2danny/extra/c35/src/Menu.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Menu.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Menu.cpp$(DependSuffix): src/Menu.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Menu.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Menu.cpp$(DependSuffix) -MM "src/Menu.cpp"

$(IntermediateDirectory)/src_Menu.cpp$(PreprocessSuffix): src/Menu.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Menu.cpp$(PreprocessSuffix) "src/Menu.cpp"

$(IntermediateDirectory)/src_MiniMap.cpp$(ObjectSuffix): src/MiniMap.cpp $(IntermediateDirectory)/src_MiniMap.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sp2danny/extra/c35/src/MiniMap.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_MiniMap.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_MiniMap.cpp$(DependSuffix): src/MiniMap.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_MiniMap.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_MiniMap.cpp$(DependSuffix) -MM "src/MiniMap.cpp"

$(IntermediateDirectory)/src_MiniMap.cpp$(PreprocessSuffix): src/MiniMap.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_MiniMap.cpp$(PreprocessSuffix) "src/MiniMap.cpp"

$(IntermediateDirectory)/src_Player.cpp$(ObjectSuffix): src/Player.cpp $(IntermediateDirectory)/src_Player.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sp2danny/extra/c35/src/Player.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Player.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Player.cpp$(DependSuffix): src/Player.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Player.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Player.cpp$(DependSuffix) -MM "src/Player.cpp"

$(IntermediateDirectory)/src_Player.cpp$(PreprocessSuffix): src/Player.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Player.cpp$(PreprocessSuffix) "src/Player.cpp"

$(IntermediateDirectory)/src_Resource.cpp$(ObjectSuffix): src/Resource.cpp $(IntermediateDirectory)/src_Resource.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sp2danny/extra/c35/src/Resource.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Resource.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Resource.cpp$(DependSuffix): src/Resource.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Resource.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Resource.cpp$(DependSuffix) -MM "src/Resource.cpp"

$(IntermediateDirectory)/src_Resource.cpp$(PreprocessSuffix): src/Resource.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Resource.cpp$(PreprocessSuffix) "src/Resource.cpp"

$(IntermediateDirectory)/src_TSD.cpp$(ObjectSuffix): src/TSD.cpp $(IntermediateDirectory)/src_TSD.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sp2danny/extra/c35/src/TSD.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_TSD.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_TSD.cpp$(DependSuffix): src/TSD.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_TSD.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_TSD.cpp$(DependSuffix) -MM "src/TSD.cpp"

$(IntermediateDirectory)/src_TSD.cpp$(PreprocessSuffix): src/TSD.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_TSD.cpp$(PreprocessSuffix) "src/TSD.cpp"

$(IntermediateDirectory)/src_Tech.cpp$(ObjectSuffix): src/Tech.cpp $(IntermediateDirectory)/src_Tech.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sp2danny/extra/c35/src/Tech.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Tech.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Tech.cpp$(DependSuffix): src/Tech.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Tech.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Tech.cpp$(DependSuffix) -MM "src/Tech.cpp"

$(IntermediateDirectory)/src_Tech.cpp$(PreprocessSuffix): src/Tech.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Tech.cpp$(PreprocessSuffix) "src/Tech.cpp"

$(IntermediateDirectory)/src_Unit.cpp$(ObjectSuffix): src/Unit.cpp $(IntermediateDirectory)/src_Unit.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sp2danny/extra/c35/src/Unit.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Unit.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Unit.cpp$(DependSuffix): src/Unit.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Unit.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Unit.cpp$(DependSuffix) -MM "src/Unit.cpp"

$(IntermediateDirectory)/src_Unit.cpp$(PreprocessSuffix): src/Unit.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Unit.cpp$(PreprocessSuffix) "src/Unit.cpp"

$(IntermediateDirectory)/src_UnitAction.cpp$(ObjectSuffix): src/UnitAction.cpp $(IntermediateDirectory)/src_UnitAction.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sp2danny/extra/c35/src/UnitAction.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_UnitAction.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_UnitAction.cpp$(DependSuffix): src/UnitAction.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_UnitAction.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_UnitAction.cpp$(DependSuffix) -MM "src/UnitAction.cpp"

$(IntermediateDirectory)/src_UnitAction.cpp$(PreprocessSuffix): src/UnitAction.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_UnitAction.cpp$(PreprocessSuffix) "src/UnitAction.cpp"

$(IntermediateDirectory)/src_pcxLoad.cpp$(ObjectSuffix): src/pcxLoad.cpp $(IntermediateDirectory)/src_pcxLoad.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sp2danny/extra/c35/src/pcxLoad.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_pcxLoad.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_pcxLoad.cpp$(DependSuffix): src/pcxLoad.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_pcxLoad.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_pcxLoad.cpp$(DependSuffix) -MM "src/pcxLoad.cpp"

$(IntermediateDirectory)/src_pcxLoad.cpp$(PreprocessSuffix): src/pcxLoad.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_pcxLoad.cpp$(PreprocessSuffix) "src/pcxLoad.cpp"

$(IntermediateDirectory)/src_resize++.cpp$(ObjectSuffix): src/resize++.cpp $(IntermediateDirectory)/src_resize++.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sp2danny/extra/c35/src/resize++.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_resize++.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_resize++.cpp$(DependSuffix): src/resize++.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_resize++.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_resize++.cpp$(DependSuffix) -MM "src/resize++.cpp"

$(IntermediateDirectory)/src_resize++.cpp$(PreprocessSuffix): src/resize++.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_resize++.cpp$(PreprocessSuffix) "src/resize++.cpp"

$(IntermediateDirectory)/src_sdlGuiColl.cpp$(ObjectSuffix): src/sdlGuiColl.cpp $(IntermediateDirectory)/src_sdlGuiColl.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sp2danny/extra/c35/src/sdlGuiColl.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_sdlGuiColl.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_sdlGuiColl.cpp$(DependSuffix): src/sdlGuiColl.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_sdlGuiColl.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_sdlGuiColl.cpp$(DependSuffix) -MM "src/sdlGuiColl.cpp"

$(IntermediateDirectory)/src_sdlGuiColl.cpp$(PreprocessSuffix): src/sdlGuiColl.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_sdlGuiColl.cpp$(PreprocessSuffix) "src/sdlGuiColl.cpp"

$(IntermediateDirectory)/src_stdafx.cpp$(ObjectSuffix): src/stdafx.cpp $(IntermediateDirectory)/src_stdafx.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sp2danny/extra/c35/src/stdafx.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_stdafx.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_stdafx.cpp$(DependSuffix): src/stdafx.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_stdafx.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_stdafx.cpp$(DependSuffix) -MM "src/stdafx.cpp"

$(IntermediateDirectory)/src_stdafx.cpp$(PreprocessSuffix): src/stdafx.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_stdafx.cpp$(PreprocessSuffix) "src/stdafx.cpp"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) ./bin/Debug/*$(ObjectSuffix)
	$(RM) ./bin/Debug/*$(DependSuffix)
	$(RM) $(OutputFile)
	$(RM) ".build-debug/c35"


