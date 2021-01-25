# On Namespaces

This library uses a fairly strange way to encode namespaces that I've found useful.

Consider a component named `foo`. It'll be represented internally with:

```
    bad::foo
    bad::foo::common
    bad::foo::api
```

Most users can probably get away with using the top level `bad` namespace alone. The goal is that
it should come "batteries included". To that end, `bad` gets cluttered up only with what is in
`bad::foo::common`. This may not be everything in the public facing APi for this component.

Advanced users can add a `using namespace bad::foo::api` to use the full public API of the component. This
contains everything in `bad::foo::common`.

If you want to be careful about namespace hygiene, you can avoid importing the top level namespace,
which will get cluttered from several components, and just importthe `bad:foo::common` or `bad::foo::api`

Usually you see `foo`, and `foo::detail`, but inverting these two allows me to avoid having to either
explicitly index into the detail namespace everywhere I want to use a private component while defining the
public API, or I have to individually export each and every template. Here I can have bad:foo indicate
treat common and api as `inline namespaces`.

I don't actually use inline namespaces because they give redefinition warnings when you re-open them as
non-inline namespaces, so I use the older approximation that was available in C++11 of replacing

    inline namespace foo {}

with 

    namespace foo {}
    using namespace foo;

This is mostly because C++17 lets you quickly bind several namespaces with `namespace bad::foo::common {}`
but it isn't until C++20 that you'll be able to mix in the inline specifiers into that list.

In practice this amounts to writing a prelude like


    namespace bad {
      /** <description> */
      namespace foo {
        /** re-exported by \ref bad and \ref bad::foo::api */
        namespace common {};
        /** public api */
        namespace api { using namespace common; }
        using namespace api;
      }
      using namespace foo::common;
    }

at the top of each component as boilerplate and then defining things directly into 

* `namespace bad::foo` for private things
* `namespace bad::foo::common` for _very_ public things we want to litter `bad` with.
* `namespace bad::foo::api` for the public api

But now internal to the component each of these namespaces can see each other. and bad::foo can freely
`using namespace` other components' `api` namespaces (or even their internals) and I
don't have to fight with namespaces all day long.

This should probably become a macro.

--Edward
