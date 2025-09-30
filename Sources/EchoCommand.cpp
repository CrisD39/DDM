class EchoCommand : public BaseCommand {
public:
    EchoCommand(CommandContext& ctx) : BaseCommand(ctx) {}
    QString getName() const override { return "echo"; }
    QString getDescription() const override { return "Imprime argumentos"; }
    QString usage() const override { return "echo [texto]"; }

    CommandResult execute() const override {
        ctx_.out << invocation_.args.join(" ") << "\n";
        ctx_.out.flush();
        return {true, ""};
    }

    void setInvocation(const CommandInvocation& inv) { invocation_ = inv; }

private:
    CommandInvocation invocation_;
};
