newaction {
	trigger = "clean",
	description = "Cleans the workspace.",
	execute = function()
		-- The directories to delete
		local directories = {
			"./.vs",
			"./bin",
			"./obj"
		}
		
		-- The files to delete
		local files = {
			"./AA_Playground.vcxproj",
			"./AA_Playground.vcxproj.filters",
			"./AA_Playground.vcxproj.user",
			"./AA_Playground_Workspace.sln",
			"./AA_Playground_Workspace.VC.db",
			"./AA_Playground_Workspace.VC.VC.opendb"
		}
		
		
		-- You shouldn't need to edit anything below this line
		local function delete(dir, typ)
			if not typ[2](dir) then
				return 0
			end
			
			os.rmdir(dir)
			typ[1](dir)

			if typ[2](dir) then
				print("Unable to delete ".. typ[3] .." \"".. dir .."\"")
				return 1
			else 
				print("Deleting ".. typ[3] .." \"".. dir .."\"")
			end
			
			return 0
		end
		
		local typeDirectory = {os.rmdir, os.isdir, "directory"}
		local typeFile = {os.remove, os.isfile, "file"}
		
		local success = 0
		print("Cleaning...")
		
		for k,v in pairs(directories) do
			success = success + delete(v, typeDirectory)
		end
		
		for k,v in pairs(files) do
			success = success + delete(v, typeFile)
		end

		if success == 0 then
			print("Cleaning completed.")
		else
			print("Clean was unable to delete some directories.")
		end
	end
}