import os
import pickle
env_file = "pieces.pkl"
def load_project_meta():
	env = {}

	# Attempt to load the map from the pickle file
	if os.path.exists(env_file):
		with open(env_file, "rb") as pickle_file:
			env = pickle.load(pickle_file)
		print("Loaded existing overlay map:", env)
	return env

def get_val(env, key):
	if key in env.keys():
		return env[key]
	return None

def save_project_meta(env):
	with open(env_file, "wb") as pickle_file:
		pickle.dump(env, pickle_file)
