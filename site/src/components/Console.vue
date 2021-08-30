<template>
    <div>
        <prism-editor
            class="editor"
            v-model="code"
            :highlight="highlighter"
            :code="code"
            :tabSize="4"
            :readonly="true"
        ></prism-editor>
    </div>
</template>

<script>
import { PrismEditor } from "vue-prism-editor";
import { highlight, languages } from "prismjs/components/prism-core";

export default {
    name: "Console",
    data: () => ({
        code: `Python 2.7.18 (default, Jul 14 2021, 08:11:37)
[GCC 10.2.1 20210110] on linux2
Type "help", "copyright", "credits" or "license" for more information.
>>> print("I was too lazy to find a Lua prompt")
>>>`,
    }),
    components: {
        PrismEditor,
    },
    methods: {
        highlighter(code) {
            console.log(languages);
            return highlight(code, languages.txt);
        },
    },
};
</script>

<style scoped lang="scss">
.editor {
    font-family: Fira code, Fira Mono, Consolas, Menlo, Courier, monospace;
    line-height: 1.5;
    color: white;
}
</style>

<style lang="css">
.prism-editor__textarea:focus {
    outline: none;
}
</style>
