(alias
 (name   runtest)
 (deps   (:x README.md)
         (:y1 ../../examples/code/introduction/hello.ml)
         (:y0 ../../examples/code/introduction/world.ml)
         prelude.ml)
 (action (progn
           (run ocaml-mdx test --prelude=prelude.ml --direction=to-md %{x})
           (diff? %{x} %{x}.corrected)
           (diff? %{y1} %{y1}.corrected)
           (diff? %{y0} %{y0}.corrected))))