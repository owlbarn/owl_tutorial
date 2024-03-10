---
layout: page
---

# Dataframe for Tabular Data


Dataframe is a popular way to manipulate data. It originates from R's dataframe and is widely implemented in many mainstream libraries such as Pandas. Essentially, a dataframe is simple container of the data that can be represented as a table.

Different from the matrices in numerical computing, data stored in a dataframe are not necessarily numbers but a mixture of different types. The flexibility of dataframe largely comes from the dynamic typing inherently offered in a language. Due to OCaml's static type checking, this poses greatest challenges to Owl when we were trying to introduce the similar functionality.

It becomes an art when balancing between flexibility and efficiency in designing the programming interface. This article covers the design of Dataframe module and its basic usage.


## Basic Concepts

The dataframe functionality is implemented in Owl's [Dataframe](https://github.com/owlbarn/owl/blob/master/src/base/misc/owl_dataframe.mli) module. Owl views a dataframe as a collection of time series data, and each series corresponds to one column in the table. All series must have the same length and each has a unique column head. In the following, we use series and column interchangeably.

Owl packs each series into a unified type called `series` and stores them in an array. As you can already see, dataframe is column-based so accessing columns is way more efficient than accessing rows. The Dataframe module only provides basic functionality to create, access, query, and iterate the data in a frame. We need to combine dataframe with the numerical functions in `Stats` module to reach its full capability. Essentially, Pandas is a bundle of table manipulation and basic statistical functions.


## Create Frames

Dataframes can be created in various ways. `Dataframe.make` is the core function if we can to create a frame dynamically. For example, the following code creates a frame consisting of three columns include "name", "age", and "salary" of four people.


```ocaml env=env_dataframe_1
  let name = Dataframe.pack_string_series [|"Alice"; "Bob"; "Carol"; "David"|]
  let age = Dataframe.pack_int_series [|20; 25; 30; 35|]
  let salary = Dataframe.pack_float_series [|2200.; 2100.; 2500.; 2800.|]
  let frame = Dataframe.make [|"name"; "age"; "salary"|] ~data:[|name; age; salary|]
```

If you run the code in `utop`, Owl can pretty print out the dataframe in the following format. If the frame grows too long or too wide, Owl is smart enough to truncate them automatically and present the table nicely in the toplevel.

```ocaml env=env_dataframe_1
# Owl_pretty.pp_dataframe Format.std_formatter frame;;
  +-----+---+------
    name age salary
  +-----+---+------
R0 Alice  20  2200.
R1   Bob  25  2100.
R2 Carol  30  2500.
R3 David  35  2800.
- : unit = ()
```

In fact, you do not necessarily need to pass in the data when calling `make` function. You can make an empty frame by just passing in head names.

```ocaml

  let empty_frame = Dataframe.make [|"name"; "age"; "salary"|];;

```

Try the code, and you will see Owl prints out an empty table.


## Manipulate Frames

There are a comprehensive set of table manipulation functions implemented in Dataframe module. We will go through them briefly in this section.

Now that Owl allows us to create empty frames, it certainly provides functions to dynamically add new columns.

```ocaml env=env_dataframe_1
  let job = Dataframe.pack_string_series [|"Engineer"; "Driver"; "Lecturer"; "Manager"|] in
  Dataframe.append_col frame job "job";;

  let gender = Dataframe.pack_string_series [|"female"; "male"; "female"; "male"|] in
  Dataframe.append_col frame gender "gender";;

  let location = Dataframe.pack_string_series [|"Cambridge, UK"; "Helsinki, FIN"; "London, UK"; "Prague, CZ"|] in
  Dataframe.append_col frame location "location";;
```

From the output, we can see that the "job" column has been appended to the end of the previously defined dataframe.

```ocaml env=env_dataframe_1
# Owl_pretty.pp_dataframe Format.std_formatter frame;;

  +-----+---+------+--------+------+-------------
    name age salary      job gender      location
  +-----+---+------+--------+------+-------------
R0 Alice  20  2200. Engineer female Cambridge, UK
R1   Bob  25  2100.   Driver   male Helsinki, FIN
R2 Carol  30  2500. Lecturer female    London, UK
R3 David  35  2800.  Manager   male    Prague, CZ
- : unit = ()
```

We can even concatenate two dataframes. Depending on concatenating direction, there are a couple of things worth our attention:

- when two dataframes are concatenated vertically, they must have the same number of columns and consistent column types; The head names of the first argument will be used in the new dataframe;
- when two dataframes are concatenated horizontally, they must have the same number of rows; all the columns of two dataframes must have unique names.

For example, the following code adds two new entries to the table by concatenating two dataframes vertically.


```ocaml env=env_dataframe_1
  let name = Dataframe.pack_string_series [|"Erin"; "Frank"|];;
  let age = Dataframe.pack_int_series [|22; 24|];;
  let salary = Dataframe.pack_float_series [|3600.; 5500.;|];;
  let job = Dataframe.pack_string_series [|"Researcher"; "Consultant"|];;
  let gender = Dataframe.pack_string_series [|"male"; "male"|];;
  let location = Dataframe.pack_string_series [|"New York, US"; "Beijing, CN"|];;
  let frame_1 = Dataframe.make [|"name"; "age"; "salary"; "job"; "gender"; "location"|]
                ~data:[|name; age; salary; job; gender; location|];;
  let frame_2 = Dataframe.concat_vertical frame frame_1;;
```

The new dataframe looks like the following.

```ocaml env=env_dataframe_1
# Owl_pretty.pp_dataframe Format.std_formatter frame_2;;

  +-----+---+------+----------+------+-------------
    name age salary        job gender      location
  +-----+---+------+----------+------+-------------
R0 Alice  20  2200.   Engineer female Cambridge, UK
R1   Bob  25  2100.     Driver   male Helsinki, FIN
R2 Carol  30  2500.   Lecturer female    London, UK
R3 David  35  2800.    Manager   male    Prague, CZ
R4  Erin  22  3600. Researcher   male  New York, US
R5 Frank  24  5500. Consultant   male   Beijing, CN
- : unit = ()
```


However, if you just want to append one or two rows, the previous method seems a bit overkill. Instead, you can call `Dataframe.append_row` function.


```ocaml env=env_dataframe_1
# let new_row = Dataframe.([|
    pack_string "Erin";
    pack_int 22;
    pack_float 2300.;
    pack_string "Researcher";
    pack_string "male";
    pack_string "New York, US" |])
  in
  Dataframe.append_row frame new_row;;
- : unit = ()
```

There are also functions allow you to retrieve the properties, for example:


```text

  val copy : t -> t          (* return the copy of a dataframe. *)

  val row_num : t -> int     (* return the number of rows. *)

  val col_num : t -> int     (* return the number of columns. *)

  val shape : t -> int * int (* return the shape of a dataframe. *)

  val numel : t -> int       (* return the number of elements. *)

  ...

```

The module applies several optimisation techniques to accelerate the operations on dataframes.
You can refer to the API reference for the complete function list.


## Query Frames

We can use various functions in the module to retrieve the information from a dataframe. The basic `get` and `set` function treats the dataframe like a matrix. We need to specify the row and column index to retrieve the value of an element.


```ocaml env=env_dataframe_1

# Dataframe.get frame 2 1;;
- : Dataframe.elt = Owl.Dataframe.Int 30
```

The `get_row` and `get_col` (also `get_col_by_name`) are used to obtain a complete row or column. For multiple rows and columns, there are also corresponding `get_rows` and `get_cols_by_name`.

Because each column has a name, we can also use head to retrieve information. However, we still need to pass in the row index because rows are not associated with names.

```ocaml env=env_dataframe_1
# Dataframe.get_by_name frame 2 "salary";;
- : Dataframe.elt = Owl.Dataframe.Float 2500.
```

We can use the `head` and `tail` functions to retrieve only the beginning or end of the dataframe. The results will be returned as a new dataframe. We can also use the more powerful functions like `get_slice` or `get_slice_by_name` if we are interested in the data within a dataframe. The slice definition used in these two functions is the same as that used in Owl's Ndarray modules.


```ocaml env=env_dataframe_1
# Dataframe.get_slice_by_name ([1;2], ["name"; "age"]) frame;;
- : Dataframe.t =

  +-----+---
    name age
  +-----+---
R0   Bob  25
R1 Carol  30

```

## Iterate, Map, and Filter

How can we miss the classic iteration functions in the functional programming? Dataframe includes the following methods to traverse the rows in a dataframe. We did not include any method to traverse columns because they can be simply extracted out as series then processed separately.

```text

  val iteri_row : (int -> elt array -> unit) -> t -> unit

  val iter_row :  (elt array -> unit) -> t -> unit

  val mapi_row : (int -> elt array -> elt array) -> t -> t

  val map_row : (elt array -> elt array) -> t -> t

  val filteri_row : (int -> elt array -> bool) -> t -> t

  val filter_row : (elt array -> bool) -> t -> t

  val filter_mapi_row : (int -> elt array -> elt array option) -> t -> t

  val filter_map_row : (elt array -> elt array option) -> t -> t

```

Applying these functions to a dataframe is rather straightforward. All the elements in a row are packed into `elt` type, it is a programmer's responsibility to unpack them properly in the passed-in function.

One interesting thing worth mentioning here is that there are several functions are associated with extended indexing operators. This allows us to write quite concise code in our application.


```text

  val ( .%( ) ) : t -> int * string -> elt
  (* associated with `get_by_name` *)

  val ( .%( )<- ) : t -> int * string -> elt -> unit
  (* associated with `set_by_name` *)

  val ( .?( ) ) : t -> (elt array -> bool) -> t
  (* associated with `filter_row` *)

  val ( .?( )<- ) : t -> (elt array -> bool) -> (elt array -> elt array) -> t
  (* associated with `filter_map_row` *)

  val ( .$( ) ) : t -> int list * string list -> t
  (* associated with `get_slice_by_name` *)

```

Let's present several examples to demonstrate how to use them. We can first pass in row index and head name tuple in `%()` to access cells.


```ocaml env=env_dataframe_1

  open Dataframe;;

  frame.%(1,"age");;
  (* return Bob's age. *)

  frame.%(2,"salary") <- pack_float 3000.;;
  (* change Carol's salary to 3000. *)

```

The operator `.?()` provides a shortcut to filter out the rows satisfying the passed-in predicate and returns the results in a new dataframe. For example, the following code filters out the people who are younger than 30.


```ocaml env=env_dataframe_1

# frame.?(fun r -> unpack_int r.(1) < 30);;
- : t =

  +-----+---+------+----------+------+-------------
    name age salary        job gender      location
  +-----+---+------+----------+------+-------------
R0 Alice  20  2200.   Engineer female Cambridge, UK
R1   Bob  25  2100.     Driver   male Helsinki, FIN
R2  Erin  22  2300. Researcher   male  New York, US

```

The cool thing about `.?()` is that you can chain the filters up like below. The code first filters out the people younger than 30, then further filter out whose salary is higher than 2100.


```ocaml env=env_dataframe_1

  frame.?(fun r -> unpack_int r.(1) < 30)
       .?(fun r -> unpack_float r.(2) > 2100.);;

```

It is also possible to filter out some rows then make some modifications. For example, we want to filter out those people older than 25, then raise their salary by 5%. We can achieve this in two ways. First, we can use `filter_map_row` functions.

```ocaml env=env_dataframe_1

  let predicate x =
    let age = unpack_int x.(1) in
    if age > 25 then (
      let old_salary = unpack_float x.(2) in
      let new_salary = pack_float (old_salary *. 1.1) in
      x.(2) <- new_salary;
      Some x
    )
    else
      None
  ;;

  filter_map_row predicate frame;;

```

Alternatively, we can use the `.?( )<-` indexing operator. The difference is that we now need to define two functions - one (i.e. `check` function) for checking the predicate and one (i.e. `modify` function) for modifying the passed-in rows.


```ocaml env=env_dataframe_1

  let check x = unpack_int x.(1) > 25;;

  let modify x =
    let old_salary = unpack_float x.(2) in
    let new_salary = pack_float (old_salary *. 1.1) in
    x.(2) <- new_salary;
    x;;

  frame.?(check) <- modify;;

```

Running the code will give you the same result as that of calling `filter_map_row` function, but the way of structuring code becomes slightly different.

Finally, you can also use `$.()` operator to replace `get_slice_by_name` function to retrieve a slice of dataframe.


```ocaml env=env_dataframe_1

# frame.$([0;2], ["name"; "salary"]);;
- : t =

  +-----+------
    name salary
  +-----+------
R0 Alice  2200.
R1   Bob  2100.
R2 Carol  3000.

```


## Read/Write CSV Files

CSV (Comma-Separated Values) is a common format to store tabular data. The module provides simple support to process CSV files. The two core functions are as follows.


```text

  val of_csv : ?sep:char -> ?head:string array -> ?types:string array -> string -> t

  val to_csv : ?sep:char -> t -> string -> unit

```

`of_csv` function loads a CSV file into in-memory dataframe while `to_csv` writes a dataframe into CSV file on the disk. In both functions, we can use `sep` to specify the separator, the default separator is `tab` in Owl.

For `of_csv` function, you can pass in the head names using `head` argument; otherwise the first row of the CSV file will be used as head. `types` argument is used to specify the type of each column in a CSV file. If `types` is dropped, all the column will be treated as string series by default. Note the length of both `head` and `types` must match the actual number of columns in the CSV file.

The mapping between `types` string and actual OCaml type is shown below:

- `b`: boolean values;
- `i`: integer values;
- `f`: float values;
- `s`: string values;

The following examples are in a [gist](http://gist.github.com/3de010940ab340e3d2bfb564ecd7d6ba) that contains code and several example CSV files.
The first example simply loads the `funding.csv` file into a dataframe, then pretty prints out the table.

```ocaml file=../../examples/code/dataframe/example_00.ml
let fname = "funding.csv" in
let types =  [|"s";"s";"f";"s";"s";"s";"s";"f";"s";"s"|] in
let df = Dataframe.of_csv ~sep:',' ~types fname in
Owl_pretty.pp_dataframe Format.std_formatter df
```

The result should look like this. We have truncated out some rows to save space here.

```text
  funding data in csv file

       +-----------------+-----------------+-------+---------+-------------+-----+----------+----------+--------------+------------
                permalink           company numEmps  category          city state fundedDate  raisedAmt raisedCurrency        round
       +-----------------+-----------------+-------+---------+-------------+-----+----------+----------+--------------+------------
     R0          lifelock          LifeLock     nan       web         Tempe    AZ   1-May-07   6850000.            USD            b
     R1          lifelock          LifeLock     nan       web         Tempe    AZ   1-Oct-06   6000000.            USD            a
     R2          lifelock          LifeLock     nan       web         Tempe    AZ   1-Jan-08  25000000.            USD            c
     R3       mycityfaces       MyCityFaces      7.       web    Scottsdale    AZ   1-Jan-08     50000.            USD         seed
     R4          flypaper          Flypaper     nan       web       Phoenix    AZ   1-Feb-08   3000000.            USD            a
     R5      infusionsoft      Infusionsoft    105.  software       Gilbert    AZ   1-Oct-07   9000000.            USD            a
                      ...               ...     ...       ...           ...   ...        ...        ...            ...          ...
  R1450              cozi              Cozi     26.  software       Seattle    WA   1-Jun-08   8000000.            USD            c
  R1451           trusera           Trusera     15.       web       Seattle    WA   1-Jun-07   2000000.            USD        angel
  R1452        alerts-com        Alerts.com     nan       web      Bellevue    WA   8-Jul-08   1200000.            USD            a
  R1453             myrio             Myrio     75.  software       Bothell    WA   1-Jan-01  20500000.            USD unattributed
  R1454     grid-networks     Grid Networks     nan       web       Seattle    WA  30-Oct-07   9500000.            USD            a
  R1455     grid-networks     Grid Networks     nan       web       Seattle    WA  20-May-08  10500000.            USD            b

```

The second example is slightly more complicated. It loads `estate.csv` file then filters out the some rows with two predicates. You can see how the two predicates are chained up with `.?()` indexing operator.


```ocaml file=../../examples/code/dataframe/example_01.ml
open Dataframe

let fname = "estate.csv" in
let d = (of_csv ~sep:',' fname)
  .?(fun row -> unpack_string row.(7) = "Condo")
  .?(fun row -> unpack_string row.(4) = "2")
in
Owl_pretty.pp_dataframe Format.std_formatter d
```

For more examples, please refer to the gist [dataframe.ml](https://github.com/owlbarn/owl/blob/master/examples/dataframe.ml).


## Infer Type and Separator

We want to devote a bit more text to CSV files. In the previous section, when we use `of_csv` function to load a CSV file, we explicitly pass in the separator and the types of all columns. However, both parameters are optional and can be skipped.

Dataframe is able to automatically detect the correct separator and the type of each column. Of course, it is possible that the detection mechanism fails but such probability is fairly low in many cases. Technically, Dataframe first tries a set of predefined separators to see which one can correctly separate the columns, and then it tries a sequence of types to find out which one is able to correctly unpack the elements of a column.

There are several technical things worth mentioning here:

- to be efficient, Dataframe only takes maximum the first 100 lines in the CSV file for inference;
- if there are missing values in a column of integer type, it falls back to float value because we can use `nan` to represent missing values;
- if the types have been decided based on the first 100 lines, any following lines containing the data of inconsistent type will be dropped.

With this capability, it is much easier to load a CSV to quickly investigate what is inside.

```ocaml file=../../examples/code/dataframe/example_02.ml
open Dataframe

let fname = "estate.csv" in
let df = Dataframe.of_csv fname in
Owl_pretty.pp_dataframe Format.std_formatter df
```

You can use the `Dataframe.types` function to retrieve the types of all columns in a dataframe.


## Summary

This chapter introduces the dataframe module in Owl, including its creating, manipulation, query, loading and saving, etc.
Comparing to those very mature libraries like Pandas, the Dataframe module in Owl is very young. We also try to keep its functionality minimal in the beginning to reserve enough space for future adjustment. The dataframe should only offer a minimal set of table manipulation functions, its analytical capability should come from the combination with other modules (e.g. `Stats`) in Owl.
