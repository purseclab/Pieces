mkdir -p build_ctxt
mkdir -p ./build_ctxt/ardu/
cp -r /home/arslan/projects/ardupilot/ardupilot/build/Pixhawk6C/bin/ ./build_ctxt/ardu/
mkdir -p ./build_ctxt/svf/
cp /home/arslan/projects/LBC/SVF//Debug-build/bin/* ./build_ctxt/svf/
mkdir -p ./build_ctxt/klee/
cp /home/arslan/projects/KLEE/klee/build2/bin/* ./build_ctxt/klee/
mkdir -p ./build_ctxt/runtime/
cp /home/arslan/projects/KLEE/klee/build2/runtime/lib/* ./build_ctxt/runtime/
sudo docker build -t pieces .
sudo docker run -it pieces bash
