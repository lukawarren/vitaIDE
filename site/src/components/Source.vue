<template>
    <div>
        <prism-editor
            class="editor"
            v-model="code"
            :highlight="highlighter"
            :code="code"
            :tabSize="4"
            line-numbers
        ></prism-editor>
    </div>
</template>

<script>
import { PrismEditor } from "vue-prism-editor";
import "vue-prism-editor/dist/prismeditor.min.css";
import { highlight, languages } from "prismjs/components/prism-core";
import "prismjs/components/prism-clike";
import "prismjs/components/prism-lua";
import "prismjs/components/prism-javascript";
import "prismjs/themes/prism-tomorrow.css";

export default {
    name: "Source",
    data: () => ({
        code: `-- Write your code here!
print("Hello world!")

for i=1,10 do
    print(i)
end

print("Hello world!")

for i=1,10 do
    print(i)
end

print("Hello world!")

for i=1,10 do
    print(i)
end
`,
    }),
    components: {
        PrismEditor,
    },
    methods: {
        highlighter(code) {
            return highlight(code, languages.lua);
        },
    },
};
</script>

<style scoped lang="scss">
.editor {
    font-family: Fira code, Fira Mono, Consolas, Menlo, Courier, monospace;
    line-height: 1.5;
}
</style>

<style lang="css">
.prism-editor__textarea:focus {
    outline: none;
}
</style>
